#include "Config.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
//extern WebServer server;
#if defined(AUTO_CONNECT)
#include <AutoConnectCore.h>
#endif
extern AutoConnect portal;
#if defined (IF_EC)
#include "SWITH/EC_meter.h"
extern EC_meter EC_Meter;
#endif
#if defined (IF_EC_ONE)
#include "SWITH/EC_ONE.h"
extern EC_ONE EC_Meter;
#endif
#if defined (IF_EC_2DELAY)
#include "SWITH/EC_meter2d.h"
extern EC_meter EC_Meter;
#endif
#if defined (IF_PHandTEMP)
#include "SWITH/PhAndTemperature.h"
extern PhAndTemperature PHAndTemperature;
#endif

String getPageTemplate(String title, String content);
#if defined (PAGE_DEBUG)
void handleDebug() {
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
          <span class="value-title">PH: </span>
          <span class="value" id="phValue">0.0 ph</span>
        </div>
        <div class="value-box">
          <span class="value-title">EC: </span>
          <span class="value" id="ecValue">0.0 %</span>
        </div>
      </div>
    </div>
    
    <script>
    function updateValues() {
        fetch('/debug-values')
          .then(response => response.json())
          .then(data => {
            document.getElementById('phValue').textContent = data.ph.toFixed(2) + ' ph';
            document.getElementById('ecValue').textContent = data.ec.toFixed(2) + ' %';
          });
      }
      
      // Обновляем значения каждые 2 секунды
      setInterval(updateValues, 2000);
      updateValues();
    </script>
  )";
// Форма для ввода первого числа
  content += "<form action='/input1' method='POST'>";
//  content += "Cycles EC: <input type='text' name='cycles' value='" + String(EC_Meter.fBasys) + "'>";
  content += "Основание EC: <input type='text' name='cycles' value='" + String(EC_Meter.fBasys) + "'>";
  content += "<input type='submit' value='OK1'>";
  content += "</form>";
  // Форма для ввода второго числа
  content += "<form action='/input2' method='POST'>";
//  content += "Delay  EC: <input type='text' name='delay' value='" + String(EC_Meter.fDegree) + "'>";
  content += "Степень  EC: <input type='text' name='delay' value='" + String(EC_Meter.fDegree) + "'>";
  content += "<input type='submit' value='OK2'>";
  content += "</form>";

  #if defined (IF_PHandTEMP)
  // Форма для ввода calibration
  content += "<form action='/calibration' method='POST'>";
  content += "calibration(Ph): <input type='text' name='calibration' value='" + String(PHAndTemperature.calibration) + "'>";
  content += "<input type='submit' value='OK3'>";
  content += "</form>";
  #endif

  webServer.send(200, "text/html", getPageTemplate("DebugPage", content));
}
#endif

void handleDebugValues() {
  WiFiWebServer&  webServer = portal.host();
  StaticJsonDocument<200> doc;
  doc["ph"] = PHAndTemperature.PH;
  doc["ec"] = EC_Meter.EC;
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

void handleInput1() {
  Serial.print("handleInput1");
  WiFiWebServer&  webServer = portal.host();
  #if defined (IF_EC)
  if (webServer.method() == HTTP_POST) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
  Serial.println("in1:Введенное число-1: " + String(EC_Meter.nCyclesEC));
  EC_Meter.nState = 0;
  #endif
  #if defined (IF_EC_ONE)
  if (webServer.method() == HTTP_POST) {
    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
  Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  EC_Meter.nState = 0;
  #endif
  #if defined (IF_EC_2DELAY)
  if (webServer.method() == HTTP_POST) {
//    EC_Meter.nCyclesEC = webServer.arg("cycles").toInt();
    EC_Meter.fBasys = webServer.arg("cycles").toFloat();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
//  Serial.println("Введенное число-1: " + String(EC_Meter.nCyclesEC));
  Serial.println("Введенное число-1: " + String(EC_Meter.fBasys));
  EC_Meter.nState = 0;
  #endif
}

void handleInput2() {
  Serial.print("handleInput2");
  WiFiWebServer&  webServer = portal.host();
  #if defined (IF_EC)
  if (webServer.method() == HTTP_POST) {
    EC_Meter.nDelayEC = webServer.arg("delay").toInt();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
  Serial.println("In2:Введенное число-2: " + String(EC_Meter.nDelayEC));
  EC_Meter.nState = 0;
  #endif
  #if defined (IF_EC_ONE)
  if (webServer.method() == HTTP_POST) {
    EC_Meter.nDelayEC = webServer.arg("delay").toInt();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
  Serial.println("Введенное число-2: " + String(EC_Meter.nDelayEC));
  EC_Meter.nState = 0;
  #endif
  #if defined (IF_EC_2DELAY)
  if (webServer.method() == HTTP_POST) {
//    EC_Meter.nDelayEC = webServer.arg("delay").toInt();
    EC_Meter.fDegree = webServer.arg("delay").toFloat();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
//  Serial.println("Введенное число-2: " + String(EC_Meter.nDelayEC));
  Serial.println("Введенное число-2: " + String(EC_Meter.fDegree));
  EC_Meter.nState = 0;
  #endif
}

void handleCalibration() {
  WiFiWebServer&  webServer = portal.host();
  #if defined (IF_PHandTEMP)
  if (webServer.method() == HTTP_POST) {
    PHAndTemperature.calibration = webServer.arg("calibration").toInt();
  }
  webServer.sendHeader("Location", "/debug");
  webServer.send(303);
  Serial.println("calibration: " + String(PHAndTemperature.calibration));
  #endif
}
