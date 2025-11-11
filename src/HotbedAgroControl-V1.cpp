#define nDeltaAllFilters 10000
#define nDeltaAllSensors 10000
/* main.cpp */
//
#include "Config.h"
#include <time.h>
#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
typedef WebServer WEBServer;

#include "Evgen/Smart_Config.h"
#include "Evgen/SmartDevice.hpp"
#include "Evgen/SD_OpenTherm.hpp"

#if defined (IF_EC_2DELAY)
bool bIfEcMeter = true;
bool EC1State = false;
bool EC2State = false;
bool bIfPhMeter = true;
#endif
#if defined (IF_LUX)
bool bIfLuxMeter = true;
#include "SWITCH/LUX_meter.h"
LUX_meter Lux_meter("LUX_meter");
#endif
#if defined (IF_DS18D20)
#include <DallasTemperature.h>
extern DallasTemperature sensor;
#endif
#if defined(IF_AMT1001)
#include "SWITCH/AMT1001.h"
AMT1001 Amt1001("AMT1001");
#endif

#include "fsaindcntrl10/FDgtFilter.h"
FDgtFilter DgtFilterT_Air("DgtFilterT_Air");
FDgtFilter DgtFilterH_Air("DgtFilterH_Air");
FDgtFilter DgtFilterLux("DgtFilterLux");
FDgtFilter DgtFilter18B20("DgtFilter18B20");
FDgtFilter DgtFilterEC("DgtFilterEC");
FDgtFilter DgtFilterPH("DgtFilterPH");
/************************************/
extern void setup_read_config(void);
extern void setup_web_common(void);
extern void loop_web(void);
/************************************/
class SD_Termo SmOT;

char SmartDevice::LocalUrl[24] = "";

int OTloop(void);
void loop2(void);

extern int OTDebugInfo[12];

static int OTstartSts = 0;
int LedSts = 0; //LOW

#include "SWITCH/LUX_meter.h"
#include "SWITCH/PhAndTemperature.h"
#include "SWITCH/SensorDS18D20.h"
#include "SWITCH/SensorLevel.h"

#if defined (IF_EC_2DELAY)
#include "SWITCH/EC_meter2d.h"
EC_meter EC_Meter("EC_METER2D");
#endif

PhAndTemperature PHAndTemperature("PHandTemp");
SensorDS18D20 DS18D20("DS18");
SensorLevel sensorlevel("SensLevel");
bool bIfRelay = true;
bool bIfAnySensors = true;
void relayControl();
bool led1State = false;
bool led2State = false;
bool led3State = false;
bool bIf_MQTT = true;
extern int gpioRelay1;
extern int gpioRelay2;
extern int gpioRelay3;
extern char relayStatus1;
extern char relayStatus2;
extern char relayStatus3;

bool bIfLaser = true;
/**************************************************************************
 * Сервисные функции
 *************************************************************************/
void setup() {
// Настраиваем фильтры температуры и влажности  
  DgtFilterT_Air.pVarX = &Amt1001.vt_air;    // значение от датчика температуры 
  DgtFilterT_Air.pVarY = &Amt1001.t_air;// значение после фильтра
  DgtFilterH_Air.pVarX = &Amt1001.vh_air;    // от влажности
  DgtFilterH_Air.pVarY = &Amt1001.h_air;// значение после фильтра
  DgtFilterT_Air.nN = 50;               // длина фильтра температуры
  DgtFilterH_Air.nN = 50;               // длина фильтра влажности
  
  DgtFilterLux.pVarX = &Lux_meter.L;    // значение от датчика люкс 
  DgtFilterLux.pVarY = &Lux_meter.LUX;  // значение после фильтра
  DgtFilterLux.nN = 50;                 // длина фильтра люкс

  DgtFilter18B20.pVarX = &DS18D20.sensDS18B20;
  DgtFilter18B20.pVarY = &DS18D20.DS18B20;
  DgtFilter18B20.nN = 50;
  
  DgtFilterEC.pVarX = &EC_Meter.EC_acc;
  DgtFilterEC.pVarY = &EC_Meter.EC;
  DgtFilterEC.nN = 50;
  
  DgtFilterPH.pVarX = &PHAndTemperature.PH_acc;
  DgtFilterPH.pVarY = &PHAndTemperature.PH;
  DgtFilterPH.nN = 50;

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LedSts);   // Turn the LED on (Note that LOW is the voltage level
  
  delay(2);
  Serial.begin(115200);
  Serial.println(IDENTIFY_TEXT);
  Serial.printf("Vers %d.%d.%d build %s\n",SmOT.Vers, SmOT.SubVers,SmOT.SubVers1,  SmOT.BiosDate);

  LedSts=1;
  digitalWrite(LED_BUILTIN, LedSts);   

  setup_read_config();
/*******************************************/
#if defined (IF_RELAY)
// Настройка выводов на выход
  pinMode(gpioRelay1, OUTPUT); pinMode(gpioRelay2, OUTPUT); pinMode(gpioRelay3, OUTPUT);
// Сброс всех реле в исходное состояние "отключено"
  digitalWrite(gpioRelay1, relayStatus1); digitalWrite(gpioRelay2, relayStatus2); digitalWrite(gpioRelay3, relayStatus3);
#endif
  pinMode(2, OUTPUT); 
  pinMode(12, OUTPUT); 
  pinMode(14, OUTPUT);                // разрешает работу LUX
  pinMode(22, INPUT); pinMode(23, INPUT);
// пины датчика уровня
  pinMode(18, INPUT); pinMode(19, INPUT); pinMode(21, INPUT);
#if defined (IF_DS18D20)
// начинаем работу с датчиком
  sensor.begin();
// устанавливаем разрешение датчика от 9 до 12 бит
  sensor.setResolution(12);
#endif  

// Настройка аналогового входа
  analogReadResolution(12);  // Устанавливаем разрешение АЦП в 12 бит (по умолчанию)

  setup_web_common();
  EC_Meter.setup_Ec_meter(EC_Meter.Ap_Pin, EC_Meter.An_Pin, EC_Meter.ADC_channel);
}

////////////////////////////////////////////////////////
int OTloop(void)
{   
  static int st = 1;
  int rc = 0;
  switch(st)
  {
    case 0:
    {  
        if(true)
        {  st++;  }
    }
    break;
    case 1: {   st++; } break;
    case 2:
      if(true)
      { 
        st = 0;
        rc = 1;
      }
    break;
  }
  return rc;
}

#define OT_CICLE_TIME 300
bool bIfLoop2 = false;
unsigned long lastUpdateTime = 0;
const long updateInterval = 10000;
unsigned long lastUpdateTimeEC = 0;
const long updateIntervalEC = 10;

void loop(void)
{
// Ниже запускается раз в 10 сек
  static unsigned long lastUpdateFilters = 10000;
  if (millis() - lastUpdateFilters > nDeltaAllFilters) {  // Обновляем каждые 10 мсекунд
    lastUpdateFilters = millis();
  #if defined(IF_FILTER_AMT1001)
    Amt1001.vt_air = analogRead(Amt1001.pinT);
    Amt1001.vh_air = analogRead(Amt1001.pinH);

    DgtFilterT_Air.run();         // работает фильтр температуры
    DgtFilterH_Air.run();         // работает фильтр влажности
  #else
    Amt1001.T_Air = analogRead(Amt1001.pinT);;  // без фильтров
    Amt1001.H_Air = analogRead(Amt1001.pinH);  // без фильтров
  #endif
  #if defined(IF_LUX)
    if (bIfLuxMeter) {
      DgtFilterLux.run();
    }
  #endif
  #if defined(IF_DS18D20)
    if (bIfAnySensors) {
      DgtFilter18B20.run();
    }
  #endif
  #if defined(IF_EC_2DELAY)
    if (bIfEcMeter) {
      *DgtFilterEC.pVarY = *DgtFilterEC.pVarX;
    }
    else { *DgtFilterEC.pVarY = 0.0; }
  #endif
  #if defined(IF_PHandTEMP)
    if (bIfPhMeter) {
      DgtFilterPH.run();
    }
  #endif

  }
// Управление реле
  if (bIfRelay) { relayControl(); }
// Чтение датчика уровня
  sensorlevel.run();
// Чтение управления люксметром
#if defined(IF_LUX)
  if (bIfLuxMeter) { Lux_meter.LUXMeterControl(); }
#endif
// Ниже запускается опрос датчиков раз в 10 сек
  static unsigned long lastUpdateSensors = 0;
  if (millis() - lastUpdateSensors > nDeltaAllSensors) {  // Обновляем каждые 1 секунд
#if defined(IF_AMT1001)
  // Здесь обновляем значение температуры (пример)
    lastUpdateSensors = millis();
    // Чтение значения AMT-1001
    Amt1001.run();                // работает датчик AMT1001
#endif
  if (bIfAnySensors) {
// Чтение и публикация температуры каждые 10 секунд
#if defined(IF_DS18D20)
      DS18D20.run();
#endif
    }
#if defined(IF_LUX)
    if (bIfLuxMeter) {
      Lux_meter.run();
    #if defined(DEBUG_LUX)
    Serial.printf("\nLUX(before/after filter):(%.2f/%.2f)\n", *DgtFilterLux.pVarX, *DgtFilterLux.pVarY);
    #endif
    }
#endif
  }
// Выше запускается раз в 10 сек
  static unsigned long t0=0; // t1=0;
  unsigned long t;
  int dt;
   t = millis();
   dt = t - t0;

  if(dt < OT_CICLE_TIME)
  {  loop2(); } 
  else { 
    if( bIfLoop2) { t0 = millis(); bIfLoop2 = false; }  
    else { loop2(); bIfLoop2 = true; }
  }
#if defined(IF_EC_2DELAY)
    if (bIfEcMeter) {
        EC_Meter.ECMeterControl(); // чтение EC1 и EC2
        if (millis() - lastUpdateTimeEC >= updateIntervalEC) {
          lastUpdateTimeEC = millis();
          EC_Meter.run();
          static int nSavState=0;
          if (EC_Meter.nDelayEC>1000) {
            if (nSavState != EC_Meter.nState) {
              nSavState = EC_Meter.nState;
              Serial.printf("EC2DELAY:State(%d)", EC_Meter.nState);
            }
          }
        }
    }
#endif
#if defined(IF_PHandTEMP)
  // Чтение значения PH-4502C
    if (bIfPhMeter)
      PHAndTemperature.run();
#endif
#if defined(PAGE_OSCILLOSCOPE)
// обновлявляем данные для осциллографа  
  if (millis() - lastUpdateTime >= updateInterval) {
    void updateOscSensorData();
    updateOscSensorData();
    lastUpdateTime = millis();
  }
#endif  
}

void loop2(void)
{   
    static unsigned long  t0=0; // t1=0;
    unsigned long t, dt;
     t = millis();
     dt = t - t0;
     if(LedSts) //быстро моргаем раз в мсек
     {  if(dt > 2)
        { LedSts = (LedSts+1)&0x01;
          digitalWrite(LED_BUILTIN, LedSts);   
          t0 = t;
        }
     } else {
        int wt = 500;
        if(dt > (unsigned long)wt)
        { LedSts = (LedSts+1)&0x01;
          digitalWrite(LED_BUILTIN, LedSts);   
          t0 = t;   
        }
     }
    SmOT.loop();
    loop_web();
}
