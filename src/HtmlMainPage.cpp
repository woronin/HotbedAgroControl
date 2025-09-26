#include "Config.h"
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#include <ArduinoJson.h>
#if defined(AUTO_CONNECT)
#include <AutoConnectCore.h>
#endif
#if defined (IF_LUX)
#include "SWITCH/LUX_meter.h"
extern LUX_meter Lux_meter;
#endif
#if defined (IF_EC_2DELAY)
#include "SWITCH/EC_meter2d.h"
extern EC_meter EC_Meter;
#endif
#if defined (IF_PHandTEMP)
#include "SWITCH/PhAndTemperature.h"
extern PhAndTemperature PHAndTemperature;
#endif
#if defined (IF_AMT1001)
#include "SWITCH/AMT1001.h"
extern AMT1001 Amt1001;
#endif
#if defined (IF_DS18D20)
#include "SWITCH/SensorDS18D20.h"
extern SensorDS18D20 DS18D20;
#endif
#if defined (IF_DS18D20)
#include "SWITCH/SensorLevel.h"
extern SensorLevel sensorlevel;

extern SensorDS18D20 DS18D20;
#endif

extern AutoConnect portal;

// Состояние управления
String controlState = "STOP";
#if defined (IF_RELAY)
extern bool led1State, led2State, led3State;
extern bool bIfRelay;
#endif
extern bool bIfLuxMeter, bIfEcMeter, bIfAnySensors;
String getPageTemplate(String title, String content);

// Навигационное меню
String getNavigationMenu() {
  String strRet = R"(
    <div class="nav-menu">)";
#if defined (PAGE_MQTT)
    strRet += R"(<a href="/mqtt" class="nav-button">Mqtt</a>)";
#endif        
#if defined (PAGE_OSCILLOSCOPE)
    strRet += R"(<a href="/oscilloscope" class="nav-button">Oscilloscope</a>)";
#endif        
#if defined (PAGE_DEBUG)
    strRet += R"(<a href="/debug" class="nav-button">DebugPage</a>)";
#endif        
    strRet += R"(<a href="/controlPanel" class="nav-button">Control Panel</a>
        <a href="/settings" class="nav-button">Settings</a>)";
#if defined (AUTO_CONNECT)
    strRet += AUTOCONNECT_LINK(BAR_24);
#endif
    strRet += R"(</div>)";
  return strRet;
}
  
// Общий шаблон страницы
String getPageTemplate(String title, String content) {
  String strRet = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32 - )=====" + title + R"=====(</title>
        <meta http-equiv="Content-type" content="text/html; charset=utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          * {margin: 0; padding: 0; box-sizing: border-box; }
          body {
            font-family: Arial, sans-serif; background-color: #1e1e1e;
            color: #e0e0e0; height: 100vh; display: flex;
            flex-direction: column;
          }
          header {
            background-color: #e0e0e0; padding: 15px; text-align: center;
            box-shadow: 0 2px 10px rgba(0,0,0,0.5);
          }
          h1 {color: #bb86fc; font-size: 1.5rem; margin-bottom: 10px;}
          .info {color: #9e9e9e; font-size: 0.9rem;}
          .nav-menu {
            display: flex; justify-content: center; gap: 10px;
            padding: 10px; background-color: #e0e0e0;
          }
          .nav-button {
            padding: 8px 15px; background-color: #bb86fc; color: #121212;
            text-decoration: none; border-radius: 4px; font-weight: bold;
            transition: background-color 0.3s;
          }
          .nav-button:hover {background-color: #9a67ea;}
          .content {flex: 1; padding: 20px;}
          .menu > a:link {
            position: absolute; display: inline-block; right: 12px;
            padding: 0 6px; text-decoration: none;
          }
          .menu > a:link {
            position: absolute;
            display: inline-block;
            right: 12px;
            padding: 0 6px;
            text-decoration: none;
          }
        </style>
    </head>
    <body>
        <header>
        <h1>ESP32 Hotbed Agro Control</h1>)=====";
        strRet += R"=====(<div class="info">IP: )=====" + WiFi.localIP().toString() + R"=====( | Update: 1s</div>
        </header>
        )=====" + getNavigationMenu() + R"=====(
        <div class="content">
        )=====" + content + R"=====(
        </div>)=====";
        strRet += R"=====(</body>
    </html>
    )=====";
    return strRet;
}
// 1. Запускается при сбросе сайта. 2. Устанавливает начальну страницу сайта 
void handleRootOsc() {
  WiFiWebServer&  webServer = portal.host();
  webServer.sendHeader("Location", "/settings");
  webServer.send(302, "text/plain", "Redirecting to Oscilloscope");
}

void handleControl() {
  WiFiWebServer&  webServer = portal.host();
  String ptr;
  ptr = R"(
  <style>
    html { font-family: Helvetica; display: inline-block; 
      margin: 0px auto; text-align: center;
    }
    p {font-size: 24px;color: #444444;margin-bottom: 10px;}
    h2 { margin: 0; }
    .button {
      display:inline-block; border-radius:7px; background:#73ad21; 
      margin:0 10px 0 10px; padding:10px 20px 10px 20px; text-decoration:none; 
      color:#000000;
    }
  </style>
  <script>
    function updateNumbers() {
      // Запрос на обновление чисел и состояния кнопки
      fetch("/data")
        .then(response => response.json())
        .then(data => {
          document.getElementById("randomNumber").innerText = data.randomNumber;
          document.getElementById("sensorValue").innerText = data.sensorValue;
          document.getElementById("sensorPH").innerText = data.sensorPH;
          document.getElementById("sensorEC").innerText = data.sensorEC;
          document.getElementById("sensorLUX").innerText = data.sensorLUX;
          document.getElementById("sensorDS18B20").innerText = data.sensorDS18B20;
          document.getElementById("sensorLevel").innerText = data.sensorLevel;
          document.getElementById("toggleState").innerText = data.toggleState ? "ON" : "OFF";
        });
    }
    function loadDoc() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("webpage").innerHTML =this.responseText}
      };
      xhttp.open("GET", "/", true);
      xhttp.send();
    }
    // Обновляем данные каждые 2 секунды
    setInterval(updateNumbers, 2000);
  </script>
  <body>)";
    ptr +=R"(<div id="webpage">)";
    #if defined (IF_AMT1001)
      ptr +=R"(<h2>Темп.воздуха: <span id="randomNumber">)" + String(Amt1001.T_Air) + R"(</span>°C;</h2>)";
      ptr +=R"(<h2>Влажность воздуха: <span id="sensorValue">)" + String(Amt1001.H_Air) + R"(</span>%</h2>)";
    #endif
    #if defined (IF_PHandTEMP)
      ptr +=R"(<h2>PH: <span id="sensorPH">)" + String(PHAndTemperature.PH) + R"(</span>ph</h2>)";
    #endif
    #if defined (IF_EC_2DELAY)
      ptr +=R"(<h2>EC: <span id="sensorEC">)" + String(EC_Meter.EC) + R"(</span></h2>)";
    #endif
    #if defined (IF_LUX)
      ptr +=R"(<h2>LUX: <span id="sensorLUX">)" + String(Lux_meter.LUX) + R"(</span>lx</h2>)";
    #endif
    #if defined (IF_DS18D20)
      ptr +=R"(<h2>Темп.раствора: <span id="sensorDS18B20">)" + String(DS18D20.DS18B20) + R"(</span>°C;</h2>)";
    #endif
    #if defined (IF_SENSORLEVEL)
      ptr +=R"(<h2>Уровень раствора: <span id="sensorLevel">)" + String(sensorlevel.fLevel) + R"(</span>%;</h2>)";
    #endif
      String page = PSTR("");
      page += String(F("<p>"));
      #if defined (IF_RELAY)
      if (led1State) { page += String(F("<a class=\"button\" href=\"/io?v=10\">R1=ON</a>")); }
      else { page += String(F("<a class=\"button\" href=\"/io?v=11\">R1=OFF</a>")); }
      if (led2State) { page += String(F("<a class=\"button\" href=\"/io?v=12\">R2=ON</a>")); }
      else { page += String(F("<a class=\"button\" href=\"/io?v=13\">R2=OFF</a>")); }
      if (led3State) { page += String(F("<a class=\"button\" href=\"/io?v=14\">V=ON</a>")); }
      else { page += String(F("<a class=\"button\" href=\"/io?v=15\">V=OFF</a>")); }
      #endif
      #if defined (IF_LUX)
      if (Lux_meter.FStatus1&&Lux_meter.FStatus2) { page += String(F("<a class=\"button\" href=\"/io?v=16\">Ясно</a>")); }
      else if (Lux_meter.FStatus1&&!Lux_meter.FStatus2) { page += String(F("<a class=\"button\" href=\"/io?v=17\">Пасм</a>")); }
      else  { page += String(F("<a class=\"button\" href=\"/io?v=18\">Пасм/Ясно=ERR</a>")); }
      #endif
      page += String(F("</p>"));
      ptr +=page;

    ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  String content = ptr;
  webServer.send(200, "text/html", getPageTemplate("Control Panel", content));
}

void handleSettings() {
  WiFiWebServer&  webServer = portal.host();
  String content = R"(
    <style>
      html { font-family: Helvetica; display: inline-block; 
        margin: 0px auto; text-align: center;
      }
      p {font-size: 24px;color: #444444;margin-bottom: 10px;}
      .button {
        display:inline-block; border-radius:7px; background:#73ad21;
        margin:0 10px 0 10px; padding:10px 20px 10px 20px; text-decoration:none;
        color:#000000;
      }
    </style>

    <div class="settings-panel">
      <div class="sensor-values">
        <div class="value-box">
          <div class="value-title">Температура</div>
          <div class="value" id="tempValue">0.0 °C</div>
        </div>
        <div class="value-box">
          <div class="value-title">Влажность</div>
          <div class="value" id="humValue">0.0 %</div>
        </div>
      </div>
    </div>
    
    <script>
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
  )";

  content += String(F("<p>"));
  #if defined (IF_RELAY)
  if (bIfRelay) { content += String(F("<a class=\"button\" href=\"/io?v=100\">RELAY=ON</a>")); }
  else { content += String(F("<a class=\"button\" href=\"/io?v=101\">RELAY=OFF</a>")); }
  #endif
  #if defined (IF_LUX)
  if (bIfLuxMeter) { content += String(F("<a class=\"button\" href=\"/io?v=102\">LUX_meter=ON</a>")); }
  else { content += String(F("<a class=\"button\" href=\"/io?v=103\">LUX_meter=OFF</a>")); }
  #endif
  #if defined (IF_EC_2DELAY)
  if (bIfEcMeter) { content += String(F("<a class=\"button\" href=\"/io?v=104\">EC_meter=ON</a>")); }
  else { content += String(F("<a class=\"button\" href=\"/io?v=105\">EC_meter=OFF</a>")); }
  #endif
  if (bIfAnySensors) { content += String(F("<a class=\"button\" href=\"/io?v=106\">ANY_sensor=ON</a>")); }
  else { content += String(F("<a class=\"button\" href=\"/io?v=107\">ANY_sensor=OFF</a>")); }
  content += String(F("</p>"));
  webServer.send(200, "text/html", getPageTemplate("Settings", content));
}
// Функция для обработки URL /data
void handleData() {
  WiFiWebServer&  webServer = portal.host();
  // Формируем JSON-ответ
  #if defined (MY_DEBUG)
  pPhAndTemperature->PH = pAmt1001->H_air*2;
  pEC_meter->EC = pAmt1001->H_air*3;
  pLux_meter->LUX = pAmt1001->H_air/2;
  pDS18D20->DS18B20 = pAmt1001->H_air/3;
  #endif
  String jsonResponse = "{\"randomNumber\":" 
  #if defined (IF_AMT1001)
  + String(Amt1001.T_Air) 
  + ",\"sensorValue\":" + String(Amt1001.H_Air) 
  #endif
  #if defined (IF_PHandTEMP)
  + String(",\"sensorPH\":") + String(PHAndTemperature.PH) 
  #endif
  #if defined (IF_EC_2DELAY)
  + String(",\"sensorEC\":") + String(EC_Meter.EC) 
  #endif
  #if defined (IF_LUX)
  + String(",\"sensorLUX\":") + String(Lux_meter.LUX) 
  #endif
  #if defined (IF_DS18D20)
  +
  
  ",\"sensorDS18B20\":" + String(DS18D20.DS18B20) 
  #endif
  #if defined (IF_SENSORLEVEL)
  +
  ",\"sensorLevel\":" + String(sensorlevel.fLevel) 
  #endif

  + String("}");

webServer.send(200, "application/json", jsonResponse);
  #if defined (MY_DEBUG)
  Serial.println("\n+++handleData()+++");
  #endif
}
