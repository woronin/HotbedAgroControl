#include "Config.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#if defined(AUTO_CONNECT)
#include <AutoConnectCore.h>
#endif
extern AutoConnect portal;

#if defined (IF_AMT1001)
#include "SWITCH/AMT1001.h"
extern AMT1001 Amt1001;
#endif

String getPageTemplate(String title, String content);
float currentTemp = 0;
float currentHum = 0;
#if defined (PAGE_OSCILLOSCOPE)
void handleOscilloscope() {
  WiFiWebServer&  webServer = portal.host();
  String content = R"(
  <div class="charts-container">
    <div class="chart-wrapper">
      <div class="chart-title">Temperature|PH (°C)</div>
      <div class="chart-container">
        <canvas id="tempChart"></canvas>
      </div>
    </div>
    
    <div class="chart-wrapper">
      <div class="chart-title">Humidity|EC (%)</div>
      <div class="chart-container">
        <canvas id="humChart"></canvas>
      </div>
    </div>
  </div>

  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <script>
    setTimeout(() => {
      const oscilloscopeStyle = {
        backgroundColor: 'transparent', borderColor: '#ff7597', pointRadius: 0,
        borderWidth: 2, tension: 0, fill: false
      };
      
      const gridConfig = {color: '#424242',
        borderColor: '#424242', tickColor: '#9e9e9e'
      };
      
      const tempCtx = document.getElementById('tempChart').getContext('2d');
      const tempChart = new Chart(tempCtx, {
        type: 'line',
        data: {
          labels: Array(120).fill(''),
          datasets: [{
            label: 'Temperature',
            ...oscilloscopeStyle,
            borderColor: '#ff7597',
            data: []
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          animation: { duration: 0 },
          scales: {
            x: { grid: gridConfig, ticks: { display: false }, border: { color: '#424242' } },
            y: { 
              grid: gridConfig,
              min: 0, max: 40,
              border: { color: '#424242' },
              ticks: { stepSize: 5, color: '#9e9e9e' }
            }
          },
          plugins: {
            legend: { display: false },
            tooltip: {
              mode: 'index',
              intersect: false,
              callbacks: { label: (ctx) => `${ctx.dataset.label}: ${ctx.parsed.y.toFixed(1)}°C` }
            }
          }
        }
      });
      
      const humCtx = document.getElementById('humChart').getContext('2d');
      const humChart = new Chart(humCtx, {
        type: 'line',
        data: {
          labels: Array(120).fill(''),
          datasets: [{
            label: 'Humidity',
            ...oscilloscopeStyle,
            borderColor: '#03dac6',
            data: []
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          animation: { duration: 0 },
          scales: {
            x: { grid: gridConfig, ticks: { display: false }, border: { color: '#424242' } },
            y: { 
              grid: gridConfig,
              min: 0, max: 100,
              border: { color: '#424242' },
              ticks: { stepSize: 10, color: '#9e9e9e' }
            }
          },
          plugins: {
            legend: { display: false },
            tooltip: {
              mode: 'index',
              intersect: false,
              callbacks: { label: (ctx) => `${ctx.dataset.label}: ${ctx.parsed.y.toFixed(1)}%` }
            }
          }
        }
      });
      
      function fetchData() {
        fetch('/dataosc')
          .then(response => response.json())
          .then(data => {
            tempChart.data.datasets[0].data = data.temperature;
            humChart.data.datasets[0].data = data.humidity;
            tempChart.update();
            humChart.update();
          })
          .catch(err => console.error('Error:', err));
      }
      
      setInterval(fetchData, 10000);
      fetchData();
      
      window.addEventListener('resize', () => {
        tempChart.resize(); humChart.resize();
      });
    }, 100);
  </script>
  <style>
    .charts-container {
      display: flex; flex-direction: column; gap: 20px; height: calc(100% - 20px);
    }
    .chart-wrapper {
      flex: 1; background-color: #1e1e1e; border-radius: 8px; padding: 15px;
      display: flex; flex-direction: column; box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    }
    .chart-title {color: #03dac6; margin-bottom: 10px; font-size: 1.2rem;}
    .chart-container {flex: 1; position: relative; width: 100%; min-height: 200px;}
  </style>
  )";
  
  webServer.send(200, "text/html", getPageTemplate("Oscilloscope", content));
}
// Буферы для хранения данных
const int dataPoints = 120;
float temperatureData[dataPoints] = {0};
float humidityData[dataPoints] = {0};
int dataIndex = 0;

// Обновление данных датчиков осциллографа. 
// Вызывается с заданной периодичностью из loop()
void updateOscSensorData() {
  float newTemp{0};
  float newHum{0};
  #if defined (MY_DEBUG)
    newTemp = random(50);
    newHum = random(100);
  #else
  #if defined (IF_AMT1001)
// Температура
    currentTemp = Amt1001.T_Air;
    newTemp = currentTemp;
// Влажность    
    newHum = Amt1001.H_Air;
  
    currentHum = newHum;
    newHum = currentHum;
  #endif
#endif
  
  temperatureData[dataIndex] = newTemp;
  humidityData[dataIndex] = newHum;
  dataIndex = (dataIndex + 1) % dataPoints;
  
  #if defined (MY_DEBUG)
    Serial.printf("Temp: %.1f°C, Hum: %.1f%%, State: %s\n", newTemp, newHum, controlState.c_str());
  #endif
}

void handleOscData() {
  WiFiWebServer&  webServer = portal.host();
  StaticJsonDocument<2048*2> doc;
  JsonArray tempArray = doc.createNestedArray("temperature");
  JsonArray humArray = doc.createNestedArray("humidity");
  
  for (int i = dataPoints-1; i >= 0; i--) {
    int idx = (dataIndex + i) % dataPoints;
    tempArray.add(temperatureData[idx]);
    humArray.add(humidityData[idx]);
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}
#endif //PAGE_OSCILLOSCOPE

// периодическое обновление данных осциллограммы
void handleOscCurrentValues() {
  WiFiWebServer&  webServer = portal.host();
  StaticJsonDocument<200> doc;
  doc["temperature"] = currentTemp;
  doc["humidity"] = currentHum;

  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}
