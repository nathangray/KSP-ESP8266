
/**
 * ESP8266 WiFi controller for Kerbal Space Program 
 * 
 * Uses web sockets to connect to Telemachus on the host
 * 
 * If it is not on your WiFi, it will create its own.  Connect to it,
 * and set your WiFi details.  Then it will join your WiFi network.
 * 
 * Connect to the ESP8266 using your browser to specify the target IP.
 * It takes 5-10 seconds from power on to connection.
 * 
 * This example uses a button between ground and pin 5 which will stage, 
 * and a potentiometer on A0 which controls the throttle.
 * 
 * The blue LED (Pin 2) will be on when connected to Telemachus.
 * Enable DEBUG to get status messages over the serial connection
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

// Enable for status messages over the serial connection
static const bool DEBUG = false;

// Memory addresses
static const byte MEMORY_OFFSET = 64;        // WiFi Manager takes the first 64
static const byte MEMORY_ADDRESS_SERVER = 0; // KSP server IP/name, 32 bytes

// PINS
static const byte STATUS_PIN = 2;
static const byte STAGE_PIN = 5;

// This deals with browsers connecting
ESP8266WebServer server(80);

// Communications with Telemachus
Telemachus tm;

// When we connect, we subscribe to this stuff
// If you get random issues, try reducing the rate
const static char subscribe[] PROGMEM = "{\
\"+\":[\"t.universalTime\",\"f.throttle\"],\
\"rate\":100\
}";

// Previous throttle value, so we don't need to keep sending
int throttle = 0;

// Store what we get in here
String data;

void setup() {
  // Set up pins
  pinMode(STAGE_PIN, INPUT_PULLUP);
  pinMode(STATUS_PIN, OUTPUT);
  digitalWrite(STATUS_PIN,LOW);
  delay(1000);
  digitalWrite(STATUS_PIN,HIGH);
  
  Serial.begin(115200);

  // This handles accessing your wifi network
  WiFiManager wifi;
  wifi.setDebugOutput(DEBUG);
  String ssid = "KSPConsole " + String(ESP.getChipId());
  wifi.autoConnect(ssid.c_str());
  DEBUG_PRINT("Wifi connected");

  // Setup web server to talk to browser
  MDNS.begin ( "kspconsole", WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/setip", setKSPAddress);
  server.onNotFound ( handleNotFound );
  server.begin();
  
  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  DEBUG_PRINT("Webserver ready");

  delay(1000);

  // Connect to KSP
  connect();
}

void loop() {
  // Listen for web browsers
  server.handleClient();

  // Not connected to KSP, keep trying.
  if(!tm.connected())
  {
    DEBUG_PRINT("Lost Telemachus");
    connect();
    return;
  }

  // Get data
  tm.getData(data);
  if(data.length() > 0)
  {
    Serial.println(data);
    yield();
  }

  // Throttle
  if(abs(analogRead(A0) - throttle ) > 2)
  {
    throttle = analogRead(A0);
    // Only map from 20 - 1000 to ensure there's a dead zone we can use to
    // turn the engines off / max
    tm.command(f_setThrottle,constrain(map(throttle,20,1000,0,1000)/1000.0,0,1.0));
  }

  // Stage
  if(digitalRead(STAGE_PIN) == LOW)
  {
    tm.command(f_stage);
  }
}

/**
 * Connect to KSP
 */
void connect() {
  // Indicator LED
  digitalWrite(STATUS_PIN,HIGH);
  
  String ip = getKSPAddress();
  DEBUG_PRINT("Connecting to " + ip);
  if(!tm.connect(ip))
  {
    DEBUG_PRINT("Failed.");
    // Unable to connect to host / Telemachus
    delay(5000);
    return;
  }
  DEBUG_PRINT("Connected.");
  
  delay(1000);
  tm.getData(ip);
  
  // Start with something
  tm.sendData(subscribe);
  digitalWrite(STATUS_PIN,LOW);
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

