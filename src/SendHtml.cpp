#include "Config.h"
#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
using WebServer = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#endif
#if defined (AUTO_CONNECT)
#include <AutoConnectCore.h>
#endif
extern AutoConnect portal;

#if defined (IF_LUX)
#include "SWITCH/LUX_meter.h"
extern LUX_meter Lux_meter;
extern bool bIfLuxMeter;
#endif
#if defined (IF_EC_2DELAY)
#include "SWITCH/EC_meter2d.h"
extern EC_meter EC_Meter;
extern bool bIfEcMeter;
#endif
#if defined (IF_PHandTEMP)
#include "SWITCH/PhAndTemperature.h"
extern PhAndTemperature PHAndTemperature;
#endif

#if defined (IF_RELAY)
extern bool led1State;
extern bool led2State;
extern bool led3State;
extern bool bIfRelay;
#endif
extern bool bIfAnySensors;

void handleGPIO() 
{
  WiFiWebServer&  webServer = portal.host();
  String param = webServer.arg("v");
  int n = param.toInt();
  if (9<n && n<30) {
    #if defined (IF_RELAY)
    if (bIfRelay) {
        if (webServer.arg("v") == "10") { led1State = false; }
        if (webServer.arg("v") == "11") { led1State = true; }
        if (webServer.arg("v") == "12") { led2State = false; }
        if (webServer.arg("v") == "13") { led2State = true; }
        if (webServer.arg("v") == "14") { led3State = false; }
        if (webServer.arg("v") == "15") { led3State = true; }
    }
    #endif
    #if defined (IF_LUX)
    if (bIfLuxMeter) {
        if (webServer.arg("v") == "16") { 
          Lux_meter.FStatus1 = true; 
          Lux_meter.FStatus2 = false; 
        }
        if (webServer.arg("v") == "17") { 
          Lux_meter.FStatus1 = true; 
          Lux_meter.FStatus2 = true; 
        }
        if (webServer.arg("v") == "18") { Lux_meter.FStatus2 = false; }
        if (webServer.arg("v") == "19") { Lux_meter.FStatus2 = true; }
    }
    #endif
    #if defined (IF_EC_2DELAY)
    if (bIfEcMeter) {
        if (webServer.arg("v") == "20") { EC_Meter.EC1State = false; }
        if (webServer.arg("v") == "21") { EC_Meter.EC1State = true; }
        if (webServer.arg("v") == "22") { EC_Meter.EC2State = false; }
        if (webServer.arg("v") == "23") { EC_Meter.EC2State = true; }
    }
    #endif
    void sendRedirect(String uri);
    sendRedirect("/controlPanel");
  }
  else {
    #if defined (IF_RELAY)
    if (webServer.arg("v") == "100") { bIfRelay = false; }
    if (webServer.arg("v") == "101") { bIfRelay = true; }
    #endif
    #if defined (IF_LUX)
    if (webServer.arg("v") == "102") { bIfLuxMeter = false; }
    if (webServer.arg("v") == "103") { bIfLuxMeter = true; }
    #endif
    #if defined (IF_EC_2DELAY)
    if (webServer.arg("v") == "104") { bIfEcMeter = false; }
    if (webServer.arg("v") == "105") { bIfEcMeter = true; }
    #endif
    if (webServer.arg("v") == "106") { bIfAnySensors = false; }
    if (webServer.arg("v") == "107") { bIfAnySensors = true; }
    void sendRedirect(String uri);
    sendRedirect("/settings");
  }
#if defined (MY_DEBUG)
  Serial.println("handleGPIO()");
#endif
}

void sendRedirect(String uri) 
{
  WiFiWebServer&  webServer = portal.host();
    webServer.sendHeader("Location", uri, true);
    webServer.send(302, "text/plain", "");
    webServer.client().stop();
}

// Функция для обработки URL /get
void handleGet() {
  WiFiWebServer&  webServer = portal.host();
  #if defined (IF_EC_2DELAY)
  if (webServer.hasArg("cycles")) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
    Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  }
  #endif
  webServer.sendHeader("Location", "/", true);
  webServer.send(302, "text/plain", "");
  webServer.client().stop();
  Serial.println("handleGet()");
}
