/**
 * Encapsulating talking to Kerbal Space Program vi Telemachus websocket server
 * 
 * Nathan Gray, November 2015
 */

#ifndef Telemachus_h
#define Telemachus_h

#include "Arduino.h"
#include "WiFiClient.h"

#ifndef WEBSOCKETCLIENT_H_
#include <WebSocketClient.h>
#endif

typedef void (*Message)(char* value);

// API Strings

const char f_stage[] PROGMEM = "f.stage";
const char f_setThrottle[] PROGMEM = "f.setThrottle";


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

    // Actual data
    void command(const char* command, char* value);
    void command(const char* command, float value);
    void command(const char* command);
    
    //void subscribe(char* message, Message callback);
    //void unsubscribe(char* message);
    boolean getData(String& data);
    
    void sendData(String& data);
    void sendData(const char* data);
    
  private:
    const String _command_msg = "{\"run\":[\"{c}\"]}";
    WiFiClient client;
    WebSocketClient webSocketClient;
};
#endif
