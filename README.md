# KSP-ESP8266
Using an ESP8266 and Telemachus to create a WiFi Kerbal controller.  I'm mostly focussing on buttons here, since tablets work great for display.

## Dependencies
* I'm using a [Node MCU devkit](http://www.seeedstudio.com/depot/NodeMCU-v2-Lua-based-ESP8266-development-kit-p-2415.html) v0.9, but any ESP8266 would probably work as long as you have access to all the pins (more than 8. 
* I'm programming it in the [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
* and using this [Websocket](https://github.com/morrissinger/ESP8266-Websocket) library
* https://github.com/tzapu/WiFiManager
* [Telemachus](https://github.com/richardbunt/Telemachus)

## Getting Started
1. Start with [KSP](http://kerbalspaceprogram.com) and [Telemachus](https://github.com/richardbunt/Telemachus), make sure they work.
2. Get yourself a [devkit](http://www.seeedstudio.com/depot/NodeMCU-v2-Lua-based-ESP8266-development-kit-p-2415.html)
2. Install [Arduino core](https://github.com/esp8266/Arduino), and make sure you can program it
3. Wire it up
4. Take this project & upload it
