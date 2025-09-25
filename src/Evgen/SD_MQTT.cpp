/* SD_MQTT.cpp */
#define sens_Temp
#define sens_Hum
#define sens_Lux
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#define FORMAT_ON_FAIL  true

#include <time.h>
#include "Config.h"
#include "Smart_Config.h"
#include "SmartDevice.hpp"
#include "SD_OpenTherm.hpp"

#if defined (HOMEASSIST_USE)

#include <ArduinoHA.h>
#include <PubSubClient.h>



void mqtt_start(void);
void mqtt_loop(void);
void mqtt_setup(void);

#if defined (IF_PHandTEMP)
#include "SWITH/PhAndTemperature.h"
extern PhAndTemperature PHAndTemperature;
#endif
#if defined(IF_AMT1001)
#include "SWITH/AMT1001.h"
extern AMT1001 Amt1001;
#endif
#if defined(IF_LUX)
#include "SWITH/LUX_meter.h"
extern LUX_meter Lux_meter;
extern bool bIfLuxMeter;
#endif
#if defined(IF_SENSORLEVEL)
#include "SWITH/SensorLevel.h"
extern SensorLevel sensorlevel;
#endif

extern WiFiClient tcp_client;

WiFiClient espClient;
PubSubClient client(espClient);
extern  SD_Termo SmOT;

/*******************************************************************************/
//HADevice *pHAdevice;
//HAMqtt *pMqtt;
const char * temperature_str = "temperature";
HADevice device;
HAMqtt mqtt(espClient, device,27);
#if defined (IF_AMT1001)
HASensor sensorPID_D(NULL,HANumber::PrecisionP3);
#endif
#if defined (IF_AMT1001)
HASensor sensorPID_I(NULL,HANumber::PrecisionP3);
const char * humidity_str = "humidity";
#endif
#if defined (IF_SENSORLEVEL)
HASensor sensorLevel(NULL,HANumber::PrecisionP3);
#endif
#if defined (sens_Lux)
HASensor sensorLUX(NULL,HANumber::PrecisionP3);
const char * illuminance_str = "illuminance";
#endif
#if defined (IF_PHandTEMP)
HASensor sensorPH(NULL,HANumber::PrecisionP3);
#endif
#if defined (IF_EC)
#include "SWITH/EC_meter.h"
extern EC_meter EC_Meter;
HASensor sensorEC(NULL,HANumber::PrecisionP3);
HASwitch IFEC("IFEC");
HASwitch IFPH("IFPH");
#endif
#if defined (IF_EC_ONE)
#include "SWITH/EC_ONE.h"
extern EC_ONE EC_Meter;
HASensor sensorEC(NULL,HANumber::PrecisionP3);
HASwitch IFEC("IFEC");
HASwitch IFPH("IFPH");
#endif
#if defined (IF_EC_2DELAY)
#include "SWITH/EC_meter2d.h"
extern EC_meter EC_Meter;
HASensor sensorEC(NULL,HANumber::PrecisionP3);
HASwitch IFEC("IFEC");
HASwitch IFPH("IFPH");
#endif

#if defined (IF_DS18D20)
#include "SWITH/SensorDS18D20.h"
extern SensorDS18D20 DS18D20;
HASensor sensorDS1820(NULL,HANumber::PrecisionP3);
#endif

//#define LED_PIN         0
#if defined(IF_RELAY)
extern bool led1State, led2State, led3State, bIfRelay;
HASwitch relay1("relay1");
HASwitch relay2("relay2");
HASwitch relay3("relay3");
void onSwitchCommandR1(bool state, HASwitch* sender)
{
    led1State = state; 
    sender->setState(state); // report state back to the Home Assistant
    void handleControl();
    handleControl();
}
void onSwitchCommandR2(bool state, HASwitch* sender)
{
    led2State = state; 
    sender->setState(state); // report state back to the Home Assistant
    void handleControl();
    handleControl();
}
void onSwitchCommandR3(bool state, HASwitch* sender)
{
    led3State = state; 
    sender->setState(state); // report state back to the Home Assistant
    void handleControl();
    handleControl();
}
#endif

#if defined (sens_Lux)
HASwitch ClearCloudy("ClearCloudy");
void onSwitchCommandClearCloudy(bool state, HASwitch* sender)
{
    Serial.printf("onSwitchCommandClearCloudy - %d\n", state);
#if defined (IF_LUX)
    if (state) {
        Lux_meter.FStatus1 = true; 
        Lux_meter.FStatus2 = true; 
    }
    else {
        Lux_meter.FStatus1 = true; 
        Lux_meter.FStatus2 = false; 
    }
    sender->setState(state); // report state back to the Home Assistant
#endif
}
#endif
extern bool bIfEcMeter;
void onSwitchCommandIFEC(bool state, HASwitch* sender)
{
#if defined (IF_EC)
    bIfEcMeter = state; 
    sender->setState(state); // report state back to the Home Assistant
#endif
#if defined (IF_EC_ONE)
    bIfEcMeter = state; 
    sender->setState(state); // report state back to the Home Assistant
#endif
#if defined (IF_EC_2DELAY)
    bIfEcMeter = state; 
    sender->setState(state); // report state back to the Home Assistant
#endif
}
extern bool bIfPhMeter;
void onSwitchCommandIFPH(bool state, HASwitch* sender)
{
#if defined (IF_PHandTEMP)
    bIfPhMeter = state; 
    sender->setState(state); // report state back to the Home Assistant
    Serial.printf("bIfPhMeter - %d\n", bIfPhMeter);
#endif
}
// By default HAHVAC supports only reporting of the temperature.
// You can enable feature you need using the second argument of the constructor.
// Please check the documentation of the HAHVAC class.
unsigned long lastReadAt = millis();
unsigned long lastAvailabilityToggleAt = millis();
bool lastInputState = false;
void OnMQTTconnected(void);
void OnMQTTdisconnected(void);

void onTargetTemperatureCommand(HANumeric temperature, HAHVAC* sender) {}

int statemqtt = -1;
int state_mqtt = -10000;
int attempt_mqtt = 0;

/************************************************************/
void mqtt_setup(void)
{  bool rc;
  char str[80];

  if (WiFi.status() != WL_CONNECTED)  
        return;
//   Serial.printf("SmOT.useMQTT = %d\n", SmOT.useMQTT);
   if(SmOT.useMQTT != 0x03) 
      return;

  if( mqtt.getDevicesTypesNb_toreg() > mqtt.getDevicesTypesNb())
  {
      Serial.printf("Error! Nb = %d, need be %d\n", mqtt.getDevicesTypesNb(),  mqtt.getDevicesTypesNb_toreg() );
//look at 45 HAMqtt mqtt(espClient, device,27);      
    return;
  }

   device.setUniqueIdStr(SmOT.MQTT_topic);
   device.setName(SmOT.MQTT_topic,SmOT.MQTT_devname); //должно быть static!!
  { static char str[40];
    sprintf(str,"%d.%d.%d %s" , SmOT.Vers,SmOT.SubVers,SmOT.SubVers1, SmOT.BiosDate);
    device.setSoftwareVersion(str); //должно быть static!!
    device.setConfigurationUrl(SmOT.LocalUrl);// --//--
  }
    device.enableSharedAvailability();
    device.enableLastWill();

    lastReadAt = millis();
    lastAvailabilityToggleAt = millis();
#if defined (IF_AMT1001)
    sensorPID_D.setAvailability(true);
    sensorPID_D.setNameUniqueIdStr(SmOT.MQTT_topic,"Темп.воздуха", "agr_t");
    sensorPID_D.setDeviceClass(temperature_str); 
#endif    
#if defined (IF_AMT1001)
    sensorPID_I.setAvailability(true);
    sensorPID_I.setNameUniqueIdStr(SmOT.MQTT_topic,"Влажность воздуха", "agr_h");
    sensorPID_I.setDeviceClass(humidity_str); 
#endif    
#if defined (IF_SENSORLEVEL)
    sensorLevel.setAvailability(true);
    sensorLevel.setNameUniqueIdStr(SmOT.MQTT_topic,"Уровень", "agr_lv");
    sensorLevel.setDeviceClass(humidity_str); 
#endif    
#if defined (sens_Lux)
    sensorLUX.setAvailability(true);
    sensorLUX.setNameUniqueIdStr(SmOT.MQTT_topic,"Lux", "agr_l");
    sensorLUX.setDeviceClass(illuminance_str); 
#endif    
#if defined (IF_PHandTEMP)
    sensorPH.setAvailability(true);
    sensorPH.setNameUniqueIdStr(SmOT.MQTT_topic,"PH", "agr_ph");
    sensorPH.setDeviceClass(temperature_str); 
#endif    
#if defined (IF_EC)
    sensorEC.setAvailability(true);
    sensorEC.setNameUniqueIdStr(SmOT.MQTT_topic,"EC", "agr_ec");
    sensorEC.setDeviceClass(temperature_str); 
#endif    
#if defined (IF_EC_ONE)
    sensorEC.setAvailability(true);
    sensorEC.setNameUniqueIdStr(SmOT.MQTT_topic,"EC", "agr_ec");
    sensorEC.setDeviceClass(temperature_str); 
#endif    
#if defined (IF_EC_2DELAY)
    sensorEC.setAvailability(true);
    sensorEC.setNameUniqueIdStr(SmOT.MQTT_topic,"EC", "agr_ec");
    sensorEC.setDeviceClass(temperature_str); 
#endif    
#if defined (IF_DS18D20)
    sensorDS1820.setAvailability(true);
    sensorDS1820.setNameUniqueIdStr(SmOT.MQTT_topic,"Темп.раствора", "agr_tds");
    sensorDS1820.setDeviceClass(temperature_str); 
#endif    

    mqtt.onConnected(OnMQTTconnected);
    mqtt.onDisconnected(OnMQTTdisconnected);
    SmOT.stsMQTT = 1;
    mqtt._mqtt->setSocketTimeout(1); //not work ???

    rc= mqtt.begin(SmOT.MQTT_server, SmOT.MQTT_port, SmOT.MQTT_user, SmOT.MQTT_pwd);
    if(rc == true)
    {  
//        Serial.println("lws(2)---");
        Serial.printf("mqtt.begin ok %s:%d %s %s\n", SmOT.MQTT_server, SmOT.MQTT_port, SmOT.MQTT_user, SmOT.MQTT_pwd);
        SmOT.stsMQTT = 2;
    } else {
        Serial.printf("mqtt.begin false\n");
    }
#if defined(IF_RELAY)
    relay1.setIcon("mdi:lightbulb");
    relay1.setName("Реле 1");
    relay1.onCommand(onSwitchCommandR1);
    relay1.setNameUniqueIdStr(SmOT.MQTT_topic,"R1", "agr_r1");
    
    relay2.setIcon("mdi:lightbulb");
    relay2.setName("Реле 2");
    relay2.onCommand(onSwitchCommandR2);
    relay2.setNameUniqueIdStr(SmOT.MQTT_topic,"R2", "agr_r2");
    
    relay3.setIcon("mdi:lightbulb");
    relay3.setName("Реле 3");
    relay3.onCommand(onSwitchCommandR3);
    relay3.setNameUniqueIdStr(SmOT.MQTT_topic,"R2", "agr_r2");
#endif    
#if defined (sens_Lux)
    ClearCloudy.setIcon("mdi:lightbulb");
    ClearCloudy.setName("Пасмурно/Ясно");
    ClearCloudy.onCommand(onSwitchCommandClearCloudy);
    ClearCloudy.setNameUniqueIdStr(SmOT.MQTT_topic,"СlearСloudy", "agr_СlearСloudy");
#endif    
#if defined (sens_Lux)
    IFEC.setIcon("mdi:lightbulb");
    IFEC.setName("On/Off(EC)");
    IFEC.onCommand(onSwitchCommandIFEC);
    IFEC.setNameUniqueIdStr(SmOT.MQTT_topic,"swEC", "agr_swec");
#endif    
#if defined (sens_Lux)
    IFPH.setIcon("mdi:lightbulb");
    IFPH.setName("On/Off(PH)");
    IFPH.onCommand(onSwitchCommandIFPH);
    IFPH.setNameUniqueIdStr(SmOT.MQTT_topic,"swPH", "agr_swph");
#endif    
}

void OnMQTTconnected(void)
{ 
  statemqtt = 1;
//   Serial.printf("OnMQTTconnected %d\n", statemqtt );

}
void OnMQTTdisconnected(void)
{ statemqtt = 0;
//   Serial.printf("OnMQTT disconnected %d\n", statemqtt );
}

void mqtt_start(void)
{
//   Serial.printf("(lws-1)mqtt_start SmOT.stsMQTT %d\n", SmOT.stsMQTT);
  if(SmOT.stsMQTT == 0)
  {   mqtt_setup();
  } else {
    int rc;
    rc= mqtt.begin(SmOT.MQTT_server,SmOT.MQTT_user, SmOT.MQTT_pwd);
    if(rc == true)
    { 
//        Serial.println("lws(3)---");
        Serial.printf("(1) mqtt.begin ok %s %s %s\n", SmOT.MQTT_server,SmOT.MQTT_user, SmOT.MQTT_pwd);
        SmOT.stsMQTT = 2;
//        Serial.printf("(lws-2)mqtt_start SmOT.stsMQTT %d\n", SmOT.stsMQTT);
    } else {
        Serial.printf("(1)mqtt.begin false\n");
    }
  }
}

void mqtt_loop(void)
{ char str[80];
static int st_old = -2;  
unsigned long t1;

if(SmOT.stsMQTT == 0) {   mqtt_setup();  return; }

    mqtt.loop();

    if(mqtt.isConnected())
    {   if(statemqtt != 1)
            Serial.println(F("MQTT connected"));
        statemqtt = 1;
        state_mqtt = mqtt._mqtt->state();
    } else {
        if(statemqtt != 0)
            Serial.println(F("MQTT DiSconnected"));
        statemqtt = 0;
        state_mqtt = mqtt._mqtt->state();
        delay(1);
        return; // return from   mqtt_loop() if not connected
    }

    if ((millis() - lastAvailabilityToggleAt) > SmOT.MQTT_interval*1000 || SmOT.MQTT_need_report)
    {   
#if defined (sens_Temp)
#if defined(IF_AMT1001)
    float ddd;
    ddd = Amt1001.T_Air;
    sprintf(str,"%.1f", ddd);
    sensorPID_D.setValue(str);
#endif    
#endif    
#if defined (sens_Hum)
#if defined(IF_AMT1001)
    float iii;
    iii = Amt1001.H_Air;
    sprintf(str,"%.1f", iii);
    sensorPID_I.setValue(str);
#endif    
#if defined(IF_SENSORLEVEL)
    iii = sensorlevel.fLevel;
    sprintf(str,"%.1f", iii);
    sensorLevel.setValue(str);
#endif    
#if defined (sens_Lux)
#if defined(IF_LUX)
  float lux;
  lux = Lux_meter.LUX;
//  lux=9.999;
  sprintf(str,"%.1f", lux);
  sensorLUX.setValue(str);
//  Serial.println(lux);
#endif    
#endif    
#endif      
#if defined (IF_PHandTEMP)
    float ph;
      ph = PHAndTemperature.PH;
      sprintf(str,"%.1f", ph);
      sensorPH.setValue(str);
#endif    
#if defined (IF_EC)
    float ec;
      ec = EC_Meter.EC;
      sprintf(str,"%.1f", ec);
      sensorEC.setValue(str);
#endif    
#if defined (IF_EC_ONE)
    float ec;
      ec = EC_Meter.EC;
      sprintf(str,"%.1f", ec);
      sensorEC.setValue(str);
#endif    
#if defined (IF_EC_2DELAY)
    float ec;
      ec = EC_Meter.EC;
      sprintf(str,"%.1f", ec);
      sensorEC.setValue(str);
#endif    
#if defined (IF_DS18D20)
    float ds20;
      ds20 = DS18D20.DS18B20;
      sprintf(str,"%.1f", ds20);
      sensorDS1820.setValue(str);
#endif    
        lastAvailabilityToggleAt = millis();
        SmOT.MQTT_need_report = 0;

    }
    static bool bIfInitSwith=false;
    if (!bIfInitSwith) {
//        Serial.printf("lws(mqtt_setup) - bIfLuxMeter %d\n", bIfLuxMeter);
//        Serial.printf("lws(mqtt_setup) - bIfPhMeter %d\n", bIfPhMeter);
        IFEC.setState(bIfLuxMeter, true);
        IFPH.setState(bIfPhMeter, true);
        ClearCloudy.setState(true, true);
        Lux_meter.FStatus1 = true;
        Lux_meter.FStatus2 = true;
        bIfInitSwith = true;
    }
}
/*******************************************************************************/
#endif  //HOMEASSIST_USE 
