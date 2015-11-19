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
    boolean getData(String& data);
    
    /*
    void subscribe(char* message, Message callback);
    void unsubscribe(char* message);
    */
  private:
    WiFiClient client;
    WebSocketClient webSocketClient;
};
#endif
