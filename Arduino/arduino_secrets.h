// Wi-Fi used for downloading the JSON drawing commands.
// Set to 0 to use only credentials saved through configuration mode.
// Set to 1 to enable the WIFI_SSID/WIFI_PASSWORD fallback below.
#define USE_HARDCODED_WIFI 0
#define WIFI_SSID     "change-me"
#define WIFI_PASSWORD "change-me"

// Configuration access point created when the GPIO5 button is released.
// Its password must contain at least 8 characters.
#define CONFIG_AP_SSID     "arduinoAP"
#define CONFIG_AP_PASSWORD "!passwordAP"
