#include "WiFiS3.h"
#include <EEPROM.h>
#include "arduino_secrets.h"
#include <ArduinoJson.h>
#include "src/core/core.h"
#include "src/e-paper/epd_5in79g.h"
#include "src/network/request.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

String getEerom = "";

String ssidAP = "";
String passAP = "";

String ssidName = "ssidInput";
String passName = "passInput";

bool isSaved = false;
bool connectFail = false;

bool configMode = false;
bool canClickBtn = true;

// Button
#define BUTTON_PIN 12
int newBtnState;  // the current state of button
int prevBtnState;

unsigned long epdEndTime;

// Setup time in minutes to pass before drawing EPD-picture again
unsigned long epdNextTime = 10;

// Setup url with json commands to draw EPD-picture
// std::string jsonUrl = "http://192.168.11.65/-_TRP_iot/-_e_paper_print_json/";
std::string jsonUrl = "http://192.168.11.25/json_paper/";


void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  delay(1000);

  LogTitle("Start JSON-Paper");

  // Set the LED pin mode
  pinMode(led, OUTPUT);

  // Initialize the pushbutton pin as a pull-up input
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  prevBtnState = digitalRead(BUTTON_PIN);
}


void loop() {
  if (canClickBtn) ButtonClick();
  if (!canClickBtn && !configMode && (!connectFail || isSaved)) {
    if (status != WL_CONNECTED) WiFiConnect();

    if (millis() - epdEndTime >= 60000 * epdNextTime) {
      Serial.println(String(epdNextTime) + " minutes have passed");
      EPD_5in79g_paint();
    }
  }
  if (configMode) {
    APConnect();
    updateLED();
  }
}


int EPD_5in79g_paint(void) {
  Serial.println("\r\nStarting process to draw EPD-picture\r\n");
  if (DEV_Module_Init() != 0) {
    return -1;
  }

  EPD_5in79g_Init();

  // Clear screen
  EPD_5in79g_Clear(EPD_5in79G_WHITE);
  DEV_Delay_ms(500);

  // Create a new image cache named IMAGE_BW and fill it with white
  UBYTE *BlackImage;
  UWORD Imagesize = ((EPD_5in79G_WIDTH % 4 == 0) ? (EPD_5in79G_WIDTH / 4) : (EPD_5in79G_WIDTH / 4 + 1)) * EPD_5in79G_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize / 4)) == NULL) {
    Serial.println("Failed to apply for black memory...\r\n");
    return -1;
  }

  Serial.println("\r\nCreate image with properties:");
  Paint_NewImage(BlackImage, EPD_5in79G_WIDTH / 2, EPD_5in79G_HEIGHT / 2, 0, WHITE);
  Serial.print("   • ");
  Serial.println("Width = " + String(EPD_5in79G_WIDTH / 2) + "px");
  Serial.print("   • ");
  Serial.println("Height = " + String(EPD_5in79G_HEIGHT / 2) + "px");
  Serial.print("   • ");
  Serial.println("Scale = 4");
  Paint_SetScale(4);

  // Select Image
  Serial.println("\r\nSelect image");
  Paint_SelectImage(BlackImage);

  // Clear the color of the picture
  Serial.println("Clear image frame buffer");
  Paint_Clear(WHITE);

  Serial.println("e-Paper draw from endpoint");
  draw_epd_5in79g_remote(jsonUrl);

  Serial.println("\r\nDisplay to EPD");
  EPD_5in79g_Display_Partial(BlackImage);
  DEV_Delay_ms(3000);

  Serial.println("Goto Sleep...");
  EPD_5in79g_Sleep();
  free(BlackImage);
  BlackImage = NULL;

  // Important, at least 2s delay
  DEV_Delay_ms(2000);

  // Close 5V
  Serial.println("Close 5V, Module enters 0 power consumption...");
  DEV_Module_Exit();

  Serial.println("Finished process to draw EPD-picture\r\n");
  epdEndTime = millis();
  return 0;
}


void ButtonClick() {
  // Read the state of the button:
  newBtnState = digitalRead(BUTTON_PIN);

  if (prevBtnState == LOW && newBtnState == HIGH) {
    Serial.println("The button is released");
    configMode = true;
  }

  if (prevBtnState != newBtnState || prevBtnState == HIGH) {
    canClickBtn = false;
  }
}


void updateLED() {
  // Error → constantly on
  if (connectFail) {
    digitalWrite(led, HIGH);
    return;
  }

  // Connected → off
  if (status == WL_CONNECTED) {
    digitalWrite(led, LOW);
    return;
  }

  // Flash (AP + connecting)
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
}


void LogTitle(String title) {
  int totalWidth = 52;
  int lineLength = (totalWidth - title.length()) / 2;

  String line = "";
  for (int i = 0; i < lineLength; i++) {
    line += "▬";
  }

  Serial.println();
  Serial.print(line);
  Serial.print(" ");
  Serial.print(title);
  Serial.print(" ");
  Serial.println(line);
}


void APConnect() {

  digitalWrite(led, LOW);

  if (status != WL_AP_LISTENING) {
    // Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ;  // Wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("Start Access Point Web Server");

    // Check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // Don't continue
      while (true);
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }

    // Override IP address
    WiFi.config(IPAddress(192, 48, 56, 2));

    // Print the SSID (SSID);
    Serial.println("Creating access point...");

    // Create open network. Change this line if you want to create an WEP network:
    status = WiFi.beginAP(ssid, pass);
    if (status != WL_AP_LISTENING) {
      Serial.println("Creating access point failed");
      // Don't continue
      while (true);
    }

    delay(1000);

    // Start the web server on port 80
    server.begin();

    // You're connected now, so print out the status
    printWiFiStatus();
  }


  WiFiClient client = server.available();

  if (client) {
    Serial.println("new client");
    String currentLine = "";
    int contentLength = 0;
    bool isPost = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        if (c == '\n') {

          // POST
          if (currentLine.startsWith("POST /")) {
            isPost = true;
          }

          // Content-Length
          if (currentLine.startsWith("Content-Length:")) {
            contentLength = currentLine.substring(15).toInt();
          }

          if (currentLine == "\r\n") {


            if (isPost && contentLength > 0) {
              String body = "";

              while (body.length() < contentLength) {
                if (client.available()) {
                  char c = client.read();
                  body += c;
                }
              }

              int pos1 = body.indexOf(ssidName + "=");
              int pos2 = body.indexOf("&" + passName + "=");

              if (pos1 != -1 && pos2 != -1 && pos2 > pos1 + ssidName.length() + 1) {
                Serial.print("BODY: ");
                Serial.println(urlDecode(body));

                clearEEPROM();
                saveEEPROM(body);

                isSaved = true;
              }
            }
            getEerom = readEEPROM();

            int pos1 = getEerom.indexOf(ssidName + "=");
            int pos2 = getEerom.indexOf("&" + passName + "=");

            if (pos1 != -1 && pos2 != -1 && pos2 > pos1 + ssidName.length() + 1) {
              ssidAP = urlDecode(getEerom.substring(pos1 + ssidName.length() + 1, pos2));
              passAP = urlDecode(getEerom.substring(pos2 + passName.length() + 2));
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();


  client.println("<!DOCTYPE html>");
  client.println("<html lang='en'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8' />");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0' />");
  client.println("<title>WiFi Configuration</title>");
  client.println("<style>");

            cssPrint(client);
  client.println("</style>");


            client.println("<body>");
            // client.println("<h1>JSON-Paper Webserver</h1>");
            client.println("<div class='container'>");

            if (!isSaved) {
              client.println("<form id='wifiForm' method='POST' action='/'>");
              if (currentLine.startsWith("GET /logo.svg")) {
                svgPrint(client);
                break;
              }
              client.println("<img src='/logo.svg' alt='Logo' draggable='false'>");
              client.println("<p>Wifi configuration</p>");
              client.println("<input id='ssid' type='text' name='" + ssidName + "' placeholder='SSID' required>");
              client.println("<input id='password' type='password' name='" + passName + "' placeholder='Password' required>");
              client.println("<br>");
              client.println("<input id='connectBtn' type='submit' value='Connect' />");
              client.println("</form>");
            } else {
                client.println("<div class='wifi-status'>");
                client.println("<img src='/logo.svg' alt='Logo' draggable='false'>");
                client.println("<p>Wifi configuration</p>");
                client.println("<ul>");
                client.println("<li>SSID: " + ssidAP + "</li>");
                String hidePassAP = "";
                for (int i = 0; i < passAP.length(); i++) {
                  hidePassAP += "*";
                }
                client.println("<li>Password: " + hidePassAP + "</li>");
                client.println("</ul>");
                client.println("<div class='loading-row' aria-label='Loading'>");
                client.println("<div class='spinner'></div>");
                client.println("</div>");
                client.println("<p><i>No orange light = Connected</i></p>");
                client.println("<p><i>Orange light = Not connected</i></p>");
                client.println("</div>");

                configMode = false;
            }
              client.println("</div>");

              client.println("</body>");
              client.println("</html>");
            break;
          }

          currentLine = "";
        }
      }
    }

    client.stop();
    Serial.println("client disconnected");
  }
}


void WiFiConnect() {

  digitalWrite(led, LOW);
  if (status != WL_CONNECTED) {
    WiFi.disconnect();
    delay(1000);
  }

  getEerom = readEEPROM();
  int pos1 = getEerom.indexOf(ssidName + "=");
  int pos2 = getEerom.indexOf("&" + passName + "=");

  if (pos1 != -1 && pos2 != -1 && pos2 > pos1 + ssidName.length() + 1) {
    ssidAP = urlDecode(getEerom.substring(pos1 + ssidName.length() + 1, pos2));
    passAP = urlDecode(getEerom.substring(pos2 + passName.length() + 2));
  }

  if (!isSaved) Serial.print("\n");
  // Attempt to connect to WiFi network:
  Serial.println("Trying to connect to WiFi...");
  Serial.println("   • SSID: " + ssidAP);
  String hidePassAP = "";
  for (int i = 0; i < passAP.length(); i++) {
    hidePassAP += "*";
  }
  Serial.println("   • Password: " + hidePassAP);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // Connect to WPA/WPA2 network:
  status = WiFi.begin(ssidAP.c_str(), passAP.c_str());

  delay(1000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ Connected");
    while (WiFi.localIP() == "0.0.0.0");
    Serial.print("   • IP: ");
    Serial.println(WiFi.localIP());
    connectFail = false;
    EPD_5in79g_paint();
  } else {
    Serial.println("✗ Error no connection");
    connectFail = true;
  }

  isSaved = false;
  updateLED();
}


void printWiFiStatus() {
  // Print the SSID of the network you're attached to:
  Serial.print("   • SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi shield's IP address:
  Serial.print("   • IP: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // Print where to go in a browser:
  Serial.print("Open webserver website: http://");
  Serial.println(ip);
}


void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, '\0');
  }
}


void saveEEPROM(String value) {
  for (int i = 0; i < value.length(); i++) {
    EEPROM.write(i, value[i]);
  }
  EEPROM.write(value.length(), '\0');  // afslut string
}


String readEEPROM() {
  char data[100];

  for (int i = 0; i < 100; i++) {
    data[i] = EEPROM.read(i);
    if (data[i] == '\0') break;
  }

  return String(data);
}


String urlDecode(String input) {
  String output = "";

  for (int i = 0; i < input.length(); i++) {
    if (input[i] == '%') {
      String hex = input.substring(i + 1, i + 3);
      char decodedChar = (char)strtol(hex.c_str(), NULL, 16);
      output += decodedChar;
      i += 2;
    } else if (input[i] == '+') {
      output += ' ';
    } else {
      output += input[i];
    }
  }

  return output;
}





void cssPrint(WiFiClient& client) {
  client.println(R"CSS(
    body {
      margin: 0;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      font-family: "Montserrat", sans-serif;
    }

    .container {
      display: flex;
      justify-content: center;
      align-items: center;
    }

    form,
    .wifi-status {
      width: min(420px, calc(100vw - 32px));
      background: hsl(237.05deg 10.48% 30.48% / 42%);
      background: hsl(237.05deg 33.32% 19.96% / 26%);
      padding: 3em;
      border-radius: 20px;
      border-left: 1px solid rgba(255, 255, 255, 0.3);
      border-top: 1px solid rgba(255, 255, 255, 0.3);
      backdrop-filter: blur(10px);
      box-shadow: 20px 20px 40px -6px rgba(0, 0, 0, 0.2);
      text-align: center;
      transition: all 0.2s ease-in-out;
    }

    img {
      display: block;
      width: 70%;
      margin: 0 auto 50px auto;
      user-select: none;
    }

    form p,
    .wifi-status > p:first-of-type {
      font-weight: bold;
      color: hsl(227 6% 41% / 1);
      font-size: 1.2rem;
      text-align: left;
      margin: 0 auto 20px auto;
      width: 215px;
    }

    form input {
      background: transparent;
      width: 215px;
      padding: 1em;
      margin: 0 auto 2em auto;
      border: none;
      border-left: 1px solid rgba(255, 255, 255, 0.3);
      border-top: 1px solid rgba(255, 255, 255, 0.3);
      border-radius: 5000px;
      backdrop-filter: blur(5px);
      box-shadow: 4px 4px 60px rgba(0, 0, 0, 0.2);
      color: #fff;
      font-family: "Montserrat", sans-serif;
      font-weight: 500;
      transition: all 0.2s ease-in-out;
      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
      outline: none;
      display: block;
    }

    form input:hover {
      background: rgb(0 0 0 / 10%);
      box-shadow: 4px 4px 60px 8px rgba(0, 0, 0, 0.2);
    }

    form input:focus {
      background: rgb(0 0 0 / 10%);
      box-shadow: 4px 4px 60px 8px rgba(0, 0, 0, 0.2);
    }

    form input[type="submit"] {
      margin-top: 10px;
      width: 150px;
      font-size: 1rem;
    }

    form input[type="submit"]:hover {
      cursor: pointer;
    }

    form input[type="submit"]:active {
      background: rgba(255, 255, 255, 0.2);
    }

    ::placeholder {
      font-family: "Montserrat", sans-serif;
      font-weight: 400;
      color: #fff;
      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.4);
    }

    .wifi-status {
      color: #fff;
      text-align: center;
    }

    .wifi-status > * {
      width: 215px;
      margin: 0 auto 16px auto;
      display: block;
    }

    .wifi-status img {
      width: 70%;
      margin: 0 auto 40px auto;
    }

    .wifi-status h1 {
      font-size: 1.2rem;
      font-weight: bold;
      color: hsl(227 6% 41% / 1);
      text-align: left;
      line-height: 1.3;
    }

    .wifi-status h3 {
      margin-top: 0;
      font-size: 1rem;
      color: #d1d5db;
      font-weight: 600;
      text-align: left;
    }

    .wifi-status ul {
      padding: 0 0 0 20px;
      text-align: left;
      list-style-position: outside;
    }

    .wifi-status li {
      margin: 0 0 10px 0;
      color: #000000;
      line-height: 1.4;
      word-break: break-word;
    }

    .wifi-status span,
    .wifi-status p,
    .wifi-status i {
      text-align: left;
      color: hsl(227 6% 41% / 1);
      line-height: 1.45;
    }

    .loading-row {
      justify-content: center;
      align-items: center;
      width: 197px;
      margin: 18px auto 10px auto;
      min-height: 40px;
    }

    .spinner {
      width: 32px;
      height: 32px;
      border: 4px solid rgba(255, 255, 255, 0.25);
      border-top: 4px solid #ffffff;
      border-radius: 50%;
      animation: spin 1s linear infinite;
    }

    @keyframes spin {
      0% {
        transform: rotate(0deg);
      }
      100% {
        transform: rotate(360deg);
      }
    }

  )CSS");
}

void svgPrint(WiFiClient& client) {
}
