/**
 * Encapsulating talking to Kerbal Space Program vi Telemachus websocket server
 * 
 * Nathan Gray, November 2015
 */


#include "Arduino.h"
#include "Telemachus.h"

Telemachus::Telemachus() {
  
}

Telemachus::~Telemachus() {
  client.stop();
}

boolean Telemachus::connect(String host) {
  char _host[100];  
  host.toCharArray(_host,100);
  return Telemachus::connect(_host);
}
boolean Telemachus::connect(char* host) {
  return Telemachus::connect(host, 8085);
}
boolean Telemachus::connect(char* host, int port) {
  if(!client.connect(host,port))
  {
    return false;
  }
  char path[] = "/datalink";
  webSocketClient.path = path;
  webSocketClient.host = host;
  if(!webSocketClient.handshake(client))
  {
    client.stop();
    return false;
  }
  // Start with something
  webSocketClient.sendData("{\"+\":[\"v.name\",\"a.version\"],\"rate\":1000}");
  return true;
}
boolean Telemachus::connected() {
  return client.connected();
}

boolean Telemachus::getData(String& data) {
  return webSocketClient.getData(data);
}

