#include "WiFiS3.h"
#include <EEPROM.h>
#include "arduino_secrets.h"
#include <ArduinoJson.h>
#include "src/core/core.h"
// #include "src/config/dev_config.h"
#include "src/e-paper/epd_5in79g.h"

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

bool jsonErr = false;

bool configMode = false;
bool canClickBtn = true;

// Button
#define BUTTON_PIN 12  // The Arduino UNO R4 pin connected to the button
int newBtnState;    // the current state of button
int prevBtnState;


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  delay(1000);

  LogTitle("Start JSON-Paper");

  // set the LED pin mode
  pinMode(led, OUTPUT);

  // initialize the pushbutton pin as a pull-up input
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  prevBtnState = digitalRead(BUTTON_PIN);


}


void loop() {
  if (canClickBtn) ButtonClick();
  if (!canClickBtn && !configMode && status != WL_CONNECTED && (!connectFail || isSaved)) WiFiConnect();
  if (configMode) APConnect();
  if (configMode && !isSaved) updateLED();


  //   configMode = true;
  //   isSaved = false;
  //   canClickBtn = false;

  // if (configMode) APConnect();
  // if (!canClickBtn && !configMode && status != WL_CONNECTED && (!connectFail || isSaved)) WiFiConnect();
}



int EPD_5in79g_test(void)
{
    Serial.println("EPD_5in79g_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    EPD_5in79g_Init();
    EPD_5in79g_Clear(EPD_5in79G_WHITE);
    DEV_Delay_ms(500);

    //Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_5in79G_WIDTH % 4 == 0)? (EPD_5in79G_WIDTH / 4 ):(EPD_5in79G_WIDTH / 4 + 1)) * EPD_5in79G_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize/4)) == NULL) {
        Serial.println("Failed to apply for black memory...\r\n");
        return -1;
    }

    Serial.println("NewImage:BlackImage and RYImage\r\n");
    Paint_NewImage(BlackImage, EPD_5in79G_WIDTH / 2, EPD_5in79G_HEIGHT / 2, 0, WHITE);
    Serial.println("create new iamge\r\n");
    Paint_SetScale(4);
    Serial.println("set scale to 4\r\n");

    //Select Image
    Serial.println("selected image\r\n");
    Paint_SelectImage(BlackImage);
    Serial.println("clear image frame buffer\r\n");
    Paint_Clear(WHITE);

    Serial.println("e-Paper draw from endpoint\r\n");
    draw_epd_5in79g_remote("http://192.168.11.65/-_TRP_iot/-_e_paper_print_json/");
    Serial.println("Finished call cmd");

// #if 1   // show bmp
//     Serial.println("show red bmp------------------------\r\n");
//     EPD_5in79g_Display(gImage_5in79g);
//     DEV_Delay_ms(2000);
// #endif

// #if 1   // Drawing on the image
//     //1.Select Image
//     Serial.println("SelectImage:BlackImage\r\n");
//     Paint_SelectImage(BlackImage);
//     Paint_Clear(EPD_5in79G_WHITE);

//     // 2.Drawing on the image
//     Serial.println("Drawing:BlackImage\r\n");
//     Paint_DrawPoint(10, 80, EPD_5in79G_BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
//     Paint_DrawPoint(10, 90, EPD_5in79G_YELLOW, DOT_PIXEL_2X2, DOT_STYLE_DFT);
//     Paint_DrawPoint(10, 100, EPD_5in79G_RED, DOT_PIXEL_3X3, DOT_STYLE_DFT);
//     Paint_DrawLine(20, 70, 70, 120, EPD_5in79G_BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawLine(70, 70, 20, 120, EPD_5in79G_BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//     Paint_DrawRectangle(20, 70, 70, 120, EPD_5in79G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawRectangle(80, 70, 130, 120, EPD_5in79G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//     Paint_DrawCircle(45, 95, 20, EPD_5in79G_RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawCircle(105, 95, 20, EPD_5in79G_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
//     Paint_DrawLine(85, 95, 125, 95, EPD_5in79G_BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
//     Paint_DrawLine(105, 75, 105, 115, EPD_5in79G_WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
//     Paint_DrawString_EN(10, 10, "Red, yellow, white and black", &Font16, EPD_5in79G_BLACK, EPD_5in79G_WHITE);
//     Paint_DrawString_EN(10, 30, "Four color e-Paper", &Font12, EPD_5in79G_WHITE, EPD_5in79G_RED);
//     Paint_DrawNum(10, 50, 123456, &Font12, EPD_5in79G_BLACK, EPD_5in79G_YELLOW);

//     Serial.println("EPD_Display\r\n");
//     EPD_5in79g_Display_Partial(BlackImage);
//     DEV_Delay_ms(3000);
// #endif

//     Serial.println("Clear...\r\n");
//     EPD_5in79g_Clear(EPD_5in79G_WHITE);

//     Serial.println("Goto Sleep...\r\n");
//     EPD_5in79g_Sleep();
//     free(BlackImage);
//     BlackImage = NULL;
//     DEV_Delay_ms(2000);//important, at least 2s
//     // close 5V
//     Serial.println("close 5V, Module enters 0 power consumption ...\r\n");
//     DEV_Module_Exit();
    
//     return 0;
}




void ButtonClick() {
  // read the state of the switch/button:
  newBtnState = digitalRead(BUTTON_PIN);

  if (prevBtnState == LOW && newBtnState == HIGH){
    Serial.println("The button is released");
    configMode = true;
    isSaved = false;
    WiFi.disconnect();
  }

  if (prevBtnState != newBtnState || prevBtnState == HIGH){
    canClickBtn = false;
  }
}

void updateLED() {
  // Fejl → konstant tændt
  if (connectFail) {
    digitalWrite(led, HIGH);
    return;
  }

  // Connected → slukket
  if (status == WL_CONNECTED) {
    digitalWrite(led, LOW);
    return;
  }
  
  // Blink (AP + connecting)
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
      //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("Start Access Point Web Server");

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // don't continue
      while (true);
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }

    // Override IP address
    WiFi.config(IPAddress(192,48,56,2));

    // print the SSID (SSID);
    Serial.println("Creating access point...");

    // Create open network. Change this line if you want to create an WEP network:
    status = WiFi.beginAP(ssid, pass);
    if (status != WL_AP_LISTENING) {
      Serial.println("Creating access point failed");
      // don't continue
      while (true);
    }

    delay(1000);

    // start the web server on port 80
    server.begin();

    // you're connected now, so print out the status
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
            client.println("Content-type:text/html");
            client.println();

            client.println("<body style='background:#1f272a;color:white;'>");
            client.println("<h1>JSON-Paper Webserver</h1>");

            if (!isSaved){
              client.println("<form method='POST' action='/'>");
              client.println("<input type='text' name='" + ssidName + "' placeholder='SSID' required>");
              client.println("<input type='text' name='" + passName + "' placeholder='Password' required>");
              client.println("<br>");
              client.println("<input type='submit' value='Connect'>");
              client.println("</form>");
            }else{
              client.println("<h3>Trying to connect to WiFi...</h3>");
              client.println("<ul>");
              client.println("<li>SSID: " + ssidAP + "</li>");
              String hidePassAP = "";
              for (int i = 0; i < passAP.length(); i++) {
                  hidePassAP += "*";
              }
              client.println("<li>Password: " + hidePassAP + "</li>");
              client.println("</ul>");
              client.println("<br>");
              client.println("<p><i>No orange light = Connected</i></p>");
              client.println("<p><i>Orange light = Not connected</i></p>");
              configMode = false;
            }


            client.println("</body>");
            client.println();
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
  // attempt to connect to WiFi network:
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
    while(WiFi.localIP() == "0.0.0.0");
    Serial.print("   • IP: ");
    Serial.println(WiFi.localIP());
    connectFail = false;
    EPD_5in79g_test();
  } else {
    Serial.println("✗ Error no connection");
    connectFail = true;
  }
  isSaved = false;
  updateLED();
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("   • SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  Serial.print("   • IP: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // print where to go in a browser:
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
  EEPROM.write(value.length(), '\0'); // afslut string
}

String readEEPROM()
{
  char data[100];

  for (int i = 0; i < 100; i++)
  {
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
      char decodedChar = (char) strtol(hex.c_str(), NULL, 16);
      output += decodedChar;
      i += 2;
    }
    else if (input[i] == '+') {
      output += ' ';
    }
    else {
      output += input[i];
    }
  }
  
  return output;
}




