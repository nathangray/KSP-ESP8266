/**
 * ESP8266 WiFi controller for Kerbal Space Program 
 * 
 * Uses web sockets to connect to Telemachus on the host
 */

// Connecting to WiFi & websocket
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WebSocketClient.h>

#include <EEPROM.h>

// For nice connecting to WiFi network & config
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>

// Finally, what we want to do
#include "Telemachus.h"

static const bool DEBUG = true;

// Memory addresses
static const byte MEMORY_OFFSET = 64;        // WiFi Manager takes the first 64
static const byte MEMORY_ADDRESS_SERVER = 0; // KSP server, 32 bytes

ESP8266WebServer server(80);

// Communications with Telemachus
Telemachus tm;

void setup() {
  delay(1000);
  Serial.begin(115200);

  // This handles accessing your wifi network
  WiFiManager wifi;
  // wifi.setDebugOutput(false);
  String ssid = "KSPConsole " + String(ESP.getChipId());
  wifi.autoConnect(ssid.c_str());
  DEBUG_PRINT("Wifi connected");

  MDNS.begin ( "kspconsole", WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/setip", setKSPAddress);
  server.onNotFound ( handleNotFound );
  server.begin();
  
  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  DEBUG_PRINT("Webserver ready");

  
  connect();
}

void loop() {
  // Listen for web browsers
  server.handleClient();
  
  if(!tm.connected())
  {
    DEBUG_PRINT("Lost Telemachus");
    connect();
    return;
  }
  else
  {
    String data;
    tm.getData(data);
    if(data.length() > 0)
    {
      Serial.println(data);
    }
  }
  yield();
}

/**
 * Connect to KSP
 */
void connect() {
  String ip = getKSPAddress();
  DEBUG_PRINT("Connecting to " + ip);
  if(!tm.connect(ip))
  {
    DEBUG_PRINT("Failed.");
    // Unable to connect to host / Telemachus
    delay(1000);
    return;
  }
  DEBUG_PRINT("Connected.");
}

String getKSPAddress() {
  return getEEPROMString(MEMORY_ADDRESS_SERVER,32);
}

/**
 * User entered KSP machine's address in browser
 */
void setKSPAddress() {
  DEBUG_PRINT("Setting " + server.arg("server"));
  setEEPROMString(MEMORY_ADDRESS_SERVER,32,server.arg("server"));
  
  connect();
}

String getEEPROMString(int start, int len) {
  EEPROM.begin(512);
  delay(10);
  String string = "";
  for (int i = MEMORY_OFFSET + start; i < MEMORY_OFFSET + start + len; i++) {
    string += char(EEPROM.read(i));
  }
  EEPROM.end();
  return string;
}

void setEEPROMString(int start, int len, String string) {
  EEPROM.begin(512);
  delay(10);
  int si = 0;
  for (int i = MEMORY_OFFSET + start; i < MEMORY_OFFSET + start + len; i++) {
    char c;
    if (si < string.length()) {
      c = string[si];
    } else {
      c = 0;
    }
    EEPROM.write(i, c);
    si++;
  }
  EEPROM.end();
}

/**
 * Handle connection to ESP8266 with browser
 */
void handleRoot() {

  DEBUG_PRINT("Server got client");
  char temp[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char addr[32];
  getKSPAddress().toCharArray(addr,32);

  snprintf ( temp, 1000,

"<html>\
  <head>\
    <title>Kerbal Space Program Controller</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Kerbal Space Program Controller</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <form action=\"/setip\" method=\"get\">\
    KSP address:<input name=\"server\" value=\"%s\" maxlength=\"32\"/>\
    <button>OK</button></form>\
  </body>\
</html>",

    hr, min % 60, sec % 60, addr
  );
  server.send ( 200, "text/html", temp );
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}


void DEBUG_PRINT(String text) {
  if(DEBUG) {
    Serial.print("- ");
    Serial.println(text);
  }
}
