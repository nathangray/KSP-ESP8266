/**
 * Encapsulating talking to Kerbal Space Program vi Telemachus websocket server
 * 
 * Nathan Gray, November 2015
 */


#include "Arduino.h"
#include "Telemachus.h"

char paramBuffer[32];
String c;

Telemachus::Telemachus() {
  client.setTimeout(1000);
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
  return true;
}
boolean Telemachus::connected() {
  return client.connected();
}

boolean Telemachus::getData(String& data) {
  return webSocketClient.getData(data);
}
void Telemachus::sendData(String& data) {
  return webSocketClient.sendData(data);
}
void Telemachus::command(const char* command) {
  //c = String(command);
  String send = _command_msg;
  Serial.println("WTF?");
  //send.replace("{c}",c);
  Serial.println(send);
  //Telemachus::sendData(send);
}
void Telemachus::command(const char* command, float value) {
  dtostrf(value,4,3,paramBuffer);
  return Telemachus::command(command, paramBuffer);
}
void Telemachus::command(const char* command, char* value) {
  c = String(command) + "["+String(value)+"]";
    
  String send = _command_msg;
  send.replace("{c}",c);
  Serial.println(send);
  Telemachus::sendData(send);
}

