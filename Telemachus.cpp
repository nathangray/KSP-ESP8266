/**
 * Encapsulating talking to Kerbal Space Program vi Telemachus websocket server
 * 
 * Nathan Gray, November 2015
 */


#include "Arduino.h"
#include "Telemachus.h"

char paramBuffer[128];
char commandBuffer[32];
char floatBuffer[6];
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
void Telemachus::sendData(const char* data) {
  strcpy_P(paramBuffer,data);
  return webSocketClient.sendData(paramBuffer);
}
void Telemachus::command(const char* command) {
  strcpy_P(commandBuffer,command);
  sprintf_P(paramBuffer, _command_msg, commandBuffer);
  Telemachus::sendData(paramBuffer);
}
void Telemachus::command(const char* command, float value) {
  dtostrf(value,4,3,floatBuffer);
  return Telemachus::command(command, floatBuffer);
}
void Telemachus::command(const char* command, char* value) {
  strcpy_P(commandBuffer,command);
  sprintf_P(paramBuffer, _command_msg_param, commandBuffer, value);
  Telemachus::sendData(paramBuffer);
}

