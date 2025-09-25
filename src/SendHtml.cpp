///*
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
#include "SWITH/LUX_meter.h"
extern LUX_meter Lux_meter;
extern bool bIfLuxMeter;
#endif
#if defined (IF_EC)
#include "SWITH/EC_meter.h"
extern EC_meter EC_Meter;
extern bool bIfEcMeter;
#endif
#if defined (IF_EC_ONE)
#include "SWITH/EC_ONE.h"
extern EC_ONE EC_Meter;
extern bool bIfEcMeter;
#endif
#if defined (IF_EC_2DELAY)
#include "SWITH/EC_meter2d.h"
extern EC_meter EC_Meter;
extern bool bIfEcMeter;
#endif
#if defined (IF_PHandTEMP)
#include "SWITH/PhAndTemperature.h"
extern PhAndTemperature PHAndTemperature;
#endif

//extern WebServer server;

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
    #if defined (IF_EC)
    if (bIfEcMeter) {
        if (webServer.arg("v") == "20") { EC_Meter.EC1State = false; }
        if (webServer.arg("v") == "21") { EC_Meter.EC1State = true; }
        if (webServer.arg("v") == "22") { EC_Meter.EC2State = false; }
        if (webServer.arg("v") == "23") { EC_Meter.EC2State = true; }
    }
    #endif
    #if defined (IF_EC_ONE)
    if (bIfEcMeter) {
        if (webServer.arg("v") == "20") { EC_Meter.EC1State = false; }
        if (webServer.arg("v") == "21") { EC_Meter.EC1State = true; }
        if (webServer.arg("v") == "22") { EC_Meter.EC2State = false; }
        if (webServer.arg("v") == "23") { EC_Meter.EC2State = true; }
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
    #if defined (IF_EC)
    if (webServer.arg("v") == "104") { bIfEcMeter = false; }
    if (webServer.arg("v") == "105") { bIfEcMeter = true; }
    #endif
    #if defined (IF_EC_ONE)
    if (webServer.arg("v") == "104") { bIfEcMeter = false; }
    if (webServer.arg("v") == "105") { bIfEcMeter = true; }
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
//extern int inputNumber;          // Введенное число
void handleGet() {
  WiFiWebServer&  webServer = portal.host();
  #if defined (IF_EC)
  if (webServer.hasArg("cycles")) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
    Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  }
  #endif
  #if defined (IF_EC_ONE)
  if (webServer.hasArg("cycles")) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
    Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  }
  #endif
  #if defined (IF_EC_2DELAY)
  if (webServer.hasArg("cycles")) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
    Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  }
  #endif
/*  
  if (server.hasArg("delay")) {
    cycles = server.arg("delay").toInt();
    Serial.println("Введенное число-2: " + String(delay));
  }
*/    
//  server.send(200, "text/plain", "Число получено");
  webServer.sendHeader("Location", "/", true);
  webServer.send(302, "text/plain", "");
  webServer.client().stop();
//*/  
  Serial.println("handleGet()");
}

/*
void handleControlV1() {
  String content = R"(
    <div class="control-panel">
    <div class="sensor-values">
      <div class="value-box">
        <div class="value-title">Temperature</div>
        <div class="value" id="tempValue">0.0 °C</div>
      </div>
      <div class="value-box">
        <div class="value-title">Humidity</div>
        <div class="value" id="humValue">0.0 %</div>
      </div>
    </div>
    
    <div class="control-buttons">
      <button class="btn backward" onclick="sendCommand('backward') Backward</button>
      <button class="btn stop" onclick="sendCommand('stop') Stop</button>
      <button class="btn forward" onclick="sendCommand('forward') Forward</button>
    </div>
    
    <div class="state-display">
      Current state: <span id="currentState">)" + controlState + R"(</span>
    </div>
  </div>

  <script>
    function sendCommand(cmd) {
      fetch('/control/' + cmd)
        .then(response => response.text())
        .then(text => {
          document.getElementById('currentState').textContent = text;
          updateValues();
        })
        .catch(err => console.error('Error:', err));
    }
    
    function updateValues() {
      fetch('/current-values')
        .then(response => response.json())
        .then(data => {
          document.getElementById('tempValue').textContent = data.temperature.toFixed(1) + ' °C';
          document.getElementById('humValue').textContent = data.humidity.toFixed(1) + ' %';
        });
    }
    
    // Обновляем значения каждую секунду
    setInterval(updateValues, 1000);
    updateValues();
  </script>

  <style>
    .control-panel { display: flex; flex-direction: column; gap: 30px; align-items: center;
      justify-content: center; height: 100%;
    }
    .sensor-values {display: flex; gap: 30px; margin-bottom: 20px;}
    .value-box { background-color: #1e1e1e; padding: 20px; border-radius: 8px;
      text-align: center; min-width: 150px; box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    }
    .value-title { color: #9e9e9e; font-size: 1rem; margin-bottom: 5px; }
    .value {color: #bb86fc; font-size: 1.8rem; font-weight: bold; }
    .control-buttons {display: flex; gap: 15px; margin-bottom: 20px;}
    .btn {
      padding: 12px 25px; border: none; border-radius: 6px; font-size: 1rem;
      font-weight: bold; cursor: pointer;
      transition: transform 0.2s, background-color 0.2s;
    }
    .btn:active { transform: scale(0.95); }
    .forward { background-color: #4CAF50; color: white; }
    .stop { background-color: #f44336; color: white; }
    .backward { background-color: #2196F3; color: white; }
    .state-display {font-size: 1.2rem; color: #9e9e9e;}
    #currentState {color: #bb86fc; font-weight: bold;}
    .button {
      display:inline-block; border-radius:7px; background:#73ad21;
      margin:0 10px 0 10px; padding:10px 20px 10px 20px; text-decoration:none;
      color:#000000;
    }
    h1 { margin-bottom: 1.2em; }
    h2 { margin: 0; }
  </style>
  <body>
    <div id=\"webpage\">
      <h2>Темп.воздуха: <span id=\"randomNumber\">0</span>"; ptr +="°C;</h2>
      <h2>Влажность воздуха: <span id=\"sensorValue\">0</span>"; ptr +="%</h2>
      <h2>PH: <span id=\"sensorPH\">0</span>"; ptr +="ph</h2>
      <h2>EC: <span id=\"sensorEC\">0</span>"; ptr +="</h2>
      <h2>LUX: <span id=\"sensorLUX\">0</span>"; ptr +="lx</h2>
      <h2>Темп.раствора: <span id=\"sensorDS18B20\">0</span>"; ptr +="°C;</h2>
  )";
#if defined (AUTO_CONNECT)
  content +="<div class=\"menu\">" AUTOCONNECT_LINK(BAR_24) "</div>";
  #endif
  content += R"(</body>)";

  server.send(200, "text/html", getPageTemplate("Control Panel", content));
}
*/