/**
 * Encapsulating talking to Kerbal Space Program vi Telemachus websocket server
 * 
 * Nathan Gray, November 2015
 */

#ifndef Telemachus_h
#define Telemachus_h

#include "Arduino.h"
#include "WiFiClient.h"
#include <pgmspace.h>

#ifndef WEBSOCKETCLIENT_H_
#include <WebSocketClient.h>
#endif

typedef void (*Message)(char* value);

// API Strings
const char f_stage[] PROGMEM        = "f.stage";
const char f_setThrottle[] PROGMEM  = "f.setThrottle";
const char f_throttle[] PROGMEM     = "f.throttle";
const char f_throttleUp[] PROGMEM   = "f.throttleUp";
const char f_throttleZero[] PROGMEM = "f.throttleZero";
const char f_throttleFull[] PROGMEM = "f.throttleFull";
const char f_throttleDown[] PROGMEM = "f.throttleDown";
const char f_rcs[] PROGMEM          = "f.rcs";
const char f_sas[] PROGMEM          = "f.sas";
const char f_light[] PROGMEM        = "f.light";
const char f_gear[] PROGMEM         = "f.gear";
const char f_break[] PROGMEM        = "f.break";
const char f_abort[] PROGMEM        = "f.abort";
const char f_ag1[] PROGMEM          = "f.ag1";
const char f_ag2[] PROGMEM          = "f.ag2";
const char f_ag3[] PROGMEM          = "f.ag3";
const char f_ag4[] PROGMEM          = "f.ag4";
const char f_ag5[] PROGMEM          = "f.ag5";
const char f_ag6[] PROGMEM          = "f.ag6";
const char f_ag7[] PROGMEM          = "f.ag7";
const char f_ag8[] PROGMEM          = "f.ag8";
const char f_ag9[] PROGMEM          = "f.ag9";
const char f_ag10[] PROGMEM         = "f.ag10";

// Run command
// Unfortunately, %S is not supported, so we must copy to a buffer
const prog_char _command_msg[] PROGMEM = "{\"run\":[\"%s\"]}";
const prog_char _command_msg_param[] PROGMEM = "{\"run\":[\"%s[%s]\"]}";

class Telemachus
{
  public:
    Telemachus();
    ~Telemachus();
    
    // Connection
    boolean connect(String host);
    boolean connect(char* host);
    boolean connect(char* host, int port);
    boolean connected();

    // Actual data - send one of the API strings,
    // with optional parameter
    void command(const char* command);
    void command(const char* command, char* value);
    void command(const char* command, float value);

    // Retrieve data from Telemachus
    boolean getData(String& data);

    // Raw send
    void sendData(String& data);
    void sendData(const char* data);
    
    //void subscribe(char* message, Message callback);
    //void unsubscribe(char* message);
    
  private:
    WiFiClient client;
    WebSocketClient webSocketClient;
};
#endif
