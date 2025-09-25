/* Web.cpp  UTF-8  */
#include "Config.h"
#include <WiFi.h>
#include <WebServer.h>
using WiFiWebServer = WebServer;
#define FORMAT_ON_FAIL  true
#define CONTROL_USE
#include <time.h>
#include "SmartDevice.hpp"
#include "SD_OpenTherm.hpp"

/*
  Include AutoConnectFS.h allows the sketch to retrieve the current file
  system that AutoConnect has selected. It derives a constant
  AUTOCONNECT_APPLIED_FILESYSTEM according to the definition state of
  AC_USE_SPIFFS or AC_USE_LITTLEFS in AutoConnectDefs.h.
  Also, the AutoConnectFS::FS class indicates either SPIFFS or LittleFS
  and will select the appropriate filesystem class depending on the file
  system applied to the sketch by the definition AC_USE_SPIFFS or
  AC_USE_LITTLEFS in AutoConnectDefs.h.
  You no need to change the sketch due to the file system change, declare
  the Filesystem object according to the following usage:
  
  #include <AutoConnectFS.h>
  AutoConnectFS::FS& name = AUTOCONNECT_APPLIED_FILESYSTEM;
  name.begin(AUTOCONNECT_FS_INITIALIZATION);
*/

#include <AutoConnect.h>
#include <AutoConnectFS.h>
//#include <AutoConnectWs.h>
//AutoConnectWs ws;

#if defined(IF_AMT1001)
#include "SWITH/AMT1001.h"
extern AMT1001 Amt1001;
#endif

AutoConnectFS::FS& FlashFS = AUTOCONNECT_APPLIED_FILESYSTEM;

extern  SD_Termo SmOT;
int WiFiDebugInfo[10] ={0,0,0,0,0, 0,0,0,0,0};

/*********************************/
const char* INFO_URI  = "/info";
const char* SETUP_URI = "/setup";
const char* BLOR_URI  = "/blor";
const char* CONTROL_URI  = "/control";
const char* CONTROL_TEMP  = "/temperature";

const char* SET_T_URI   = "/set_t";
const char* SET_PAR_URI = "/set_par";
const char* SET_ADD_URI = "/add";

const char* STYLE_WIDTH = "width:20%";
/************* InfoPage ******************/
ACText(Caption, "<b>Статус OT: </b>", "", "", AC_Tag_DIV);
ACText(Info1, "", "", "", AC_Tag_DIV);
ACText(Info2, "", "", "", AC_Tag_DIV);
ACText(Info3, "", "", "", AC_Tag_DIV);
ACText(Info4, "", "", "", AC_Tag_DIV);
ACText(Info5, "", "", "", AC_Tag_DIV);
ACText(Info6, "", "", "", AC_Tag_DIV);
ACText(Info7, "", "", "", AC_Tag_DIV);
ACSubmit(Apply, "Обновить", INFO_URI, AC_Tag_BR);
ACSubmit(SetNewBoilerTemp,"Задать", SET_T_URI, AC_Tag_DIV);

/************* SetupPage ***************/
ACText(Ctrl2, "", "", "", AC_Tag_DIV);
//ACCheckbox(CtrlChB1,"checkbox", "uniqueapid");
#if MQTT_USE
AutoConnectCheckbox CtrlChbUseMQTT("ChbUseMQTT","7", "MQTT", false, AC_Behind , AC_Tag_DIV);
ACInput(SetMQTT_server,"", "сервер"); 
ACInput(SetMQTT_port,"", "порт", "",  "", AC_Tag_BR, AC_Input_Number, STYLE_WIDTH); 
ACInput(SetMQTT_user,"", "user"); 
ACInput(SetMQTT_pwd,"", "pwd"); 
ACInput(SetMQTT_topic,"", "топик"); 
ACInput(SetMQTT_devname,"", "имя устройства"); 
ACInput(SetMQTT_interval,"", "интервал, сек", "",  "Введи интервал",AC_Tag_BR, AC_Input_Number, STYLE_WIDTH); 
#endif // MQTT_USE

#if defined(CONTROL_USE)
AutoConnectCheckbox CtrlChbUseRelay("ChbUseRelay","7", "Relay", false, AC_Behind , AC_Tag_DIV);
AutoConnectCheckbox CtrlChbUseEC("ChbUseEC","7", "EC", false, AC_Behind , AC_Tag_DIV);
AutoConnectCheckbox CtrlChbUsePH("ChbUsePH","7", "PH", false, AC_Behind , AC_Tag_DIV);
ACInput(Set_DelayEC,"", "Delay(EC)", "",  "", AC_Tag_BR, AC_Input_Number, STYLE_WIDTH); 
ACInput(Set_CyclesEC,"", "Cycles(EC)", "",  "", AC_Tag_BR, AC_Input_Number, STYLE_WIDTH); 
ACInput(Set_Tair,"", "Температура воздуха"); 
ACInput(Set_Hair,"", "Влажность воздуха"); 

#endif
//AutoConnectCheckbox CtrlChB_UseRemoteControl("CtrlChB5","5", "Разрешить удаленное управление", false, AC_Behind , AC_Tag_DIV);
  
ACSubmit(ApplyChB, "Задать", SET_PAR_URI, AC_Tag_DIV);
ACSubmit(ApplyChCntrl, "Применить", CONTROL_URI, AC_Tag_DIV);
//ACSubmit(ApplyAdd, "Дополнительно", SETUP_ADD_URI, AC_Tag_None);


/************* SetupAdditionPage for MConfigMMemberIDcode ***************/
ACSubmit(ApplyAddpar,   "Задать", SET_ADD_URI, AC_Tag_BR);
ACSubmit(SendBLOR, "Сброс ошибки", BLOR_URI, AC_Tag_BR);

/************* SetPID end ***************/

/************* AboutPage *****************/

/*****************************************/

// AutoConnectAux for the custom Web page.

AutoConnectAux InfoPage(INFO_URI, "AgroControl", true, { Caption, Info1, Info2, Info3, Info4, Info5, Info6, Info7,  Apply, SetNewBoilerTemp });

AutoConnectAux Setup_Page(SETUP_URI, "Setup MQTT", true, { Ctrl2,  Info2,
     CtrlChbUseMQTT, SetMQTT_user, SetMQTT_pwd, SetMQTT_server, SetMQTT_port, SetMQTT_topic, SetMQTT_devname, SetMQTT_interval, ApplyChB});

AutoConnectAux Control_Page(CONTROL_URI, "Control of Device", true, { CtrlChbUseRelay, CtrlChbUseEC, CtrlChbUsePH, Set_DelayEC, Set_CyclesEC, Set_Tair, Set_Hair, ApplyChCntrl});

AutoConnectConfig config;
AutoConnect portal;

AutoConnectAux SetParPage(SET_PAR_URI, "SetPar", false, {}, false);

/************************************/
// Пользовательская страница
AutoConnectText temperatureText("temp", "", "Current Temperature: -- °C");
AutoConnectAux temperaturePage(CONTROL_TEMP, "Temperature", true, {temperatureText});

//int test_fs(void);

void setup_web_common(void);
void check_fs(void);
int setup_web_common_onconnect(void);
void loop_web(void);
void onRoot(void);
void loadParam(String fileName);
void onConnect(IPAddress& ipaddr);
#if MQTT_USE
  extern void mqtt_setup(void);
  extern void mqtt_loop(void);
  extern void mqtt_start(void);
  extern int MQTT_pub_usePID(void);
#endif
String onInfo(AutoConnectAux& aux, PageArgument& args);
String on_Setup(AutoConnectAux& aux, PageArgument& args);
String onSetPar(AutoConnectAux& aux, PageArgument& args);
String on_Control(AutoConnectAux& aux, PageArgument& args);

String utc_time_jc;

/************************************/
unsigned int /* AutoConnect:: */ _toWiFiQuality(int32_t rssi);


#if defined(IF_EC)
#include "SWITH/EC_meter.h"
extern EC_meter EC_Meter;
float currentTemperature = 0.0;
#endif
#if defined(IF_EC_ONE)
#include "SWITH/EC_meter.h"
extern EC_meter EC_Meter;
float currentTemperature = 0.0;
#endif
#if defined(IF_EC_2DELAY)
#include "SWITH/EC_meter2d.h"
extern EC_meter EC_Meter;
float currentTemperature = 0.0;
#endif

void setup_web_common(void)
{    bool b;

  char str[40];
  {  

  #if MQTT_USE
     SetMQTT_server.value = SmOT.MQTT_server;
     SetMQTT_user.value = SmOT.MQTT_user;
     SetMQTT_pwd.value = SmOT.MQTT_pwd;
     sprintf(str,"%d",SmOT.MQTT_port);
     SetMQTT_port.value = str;

     SetMQTT_topic.value = SmOT.MQTT_topic;
     sprintf(str,"%d",SmOT.MQTT_interval);
     SetMQTT_interval.value = str;
     SetMQTT_devname.value = SmOT.MQTT_devname;
  #endif

  }

  sprintf(str,"%d",EC_Meter.nDelayEC);
  Set_DelayEC.value = str;
  sprintf(str,"%d",EC_Meter.nCyclesEC);
  Set_CyclesEC.value = str;


  InfoPage.on(onInfo);      // Register the attribute overwrite handler.
  Setup_Page.on(on_Setup);
  SetParPage.on(onSetPar);
  Control_Page.on(on_Control);

/**/  
    portal.join({InfoPage, Setup_Page, SetParPage, Control_Page});     // Join pages.

//  portal.join({InfoPage, Setup_Page, SetTempPage});     // Join pages.
  config.ota = AC_OTA_BUILTIN;
  config.portalTimeout = 1; 
  config.retainPortal = true; 
  config.autoRise = true;
  //config.hostName = AUTOCONNECT_APID;
  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.
  config.autoReconnect = true;
  config.reconnectInterval = 2; //1;
  config.menuItems = config.menuItems | AC_MENUITEM_DELETESSID;
   Serial.printf("WiFi psk=%s\n", config.psk.c_str());
  
  portal.config(config);
  portal.onConnect(onConnect);  // Register the ConnectExit function
  portal.begin();

  WiFiWebServer&  webServer = portal.host();

  void handleRootOsc();
  webServer.on("/", handleRootOsc);                        // менеджер базовой страницы сайта
    // Обработка URL /data
  void handleData();
  webServer.on("/data", handleData);                     // обновляем данные на странице контроля
#if defined (IF_EC)
  void handleGet();
  webServer.on("/get", handleGet);
  void handleInput1(); void handleInput2(); void handleCalibration();
  webServer.on("/input1", handleInput1);
  webServer.on("/input2", handleInput2);
  webServer.on("/calibration", handleCalibration);
#endif
#if defined (IF_EC_ONE)
  void handleGet();
  webServer.on("/get", handleGet);
  void handleInput1(); void handleInput2(); void handleCalibration();
  webServer.on("/input1", handleInput1);
  webServer.on("/input2", handleInput2);
  webServer.on("/calibration", handleCalibration);
#endif
#if defined (IF_EC_2DELAY)
  void handleGet();
  webServer.on("/get", handleGet);
  void handleInput1(); void handleInput2(); void handleCalibration();
  webServer.on("/input1", handleInput1);
  webServer.on("/input2", handleInput2);
  webServer.on("/calibration", handleCalibration);
#endif

  void handleOscCurrentValues();
  webServer.on("/current-values", handleOscCurrentValues);    // периодическое обновление данных сайта
  void handleDebugValues();
  webServer.on("/debug-values", handleDebugValues);           // периодическое обновление данных на странице отладки
  void handleControl();
  webServer.on("/controlPanel", handleControl);                 // страница данных
  void handleSettings();
  webServer.on("/settings", handleSettings);               // настройки
    void handleGPIO();
  webServer.on("/io", handleGPIO);                         // обработка событий от кнопок

#if defined(PAGE_OSCILLOSCOPE)
  void handleOscilloscope();
  webServer.on("/oscilloscope", handleOscilloscope);       // осциллограф
  void handleOscData();
  webServer.on("/dataosc", handleOscData);                 // данные для осциллографа
#endif
#if defined(PAGE_DEBUG)
  void handleDebug();
  webServer.on("/debug", handleDebug);                    // страница отладки
#endif


  if (WiFi.status() != WL_CONNECTED)  {
    Serial.println(F("WiFi Not connected"));
    WiFi.setAutoReconnect(true);
  }  
  
/* get my MAC*/
    if(WiFi.getMode() == WIFI_MODE_NULL){
        esp_read_mac(SmOT.Mac, ESP_MAC_WIFI_STA);
    }
    else{
        esp_wifi_get_mac(WIFI_IF_STA, SmOT.Mac);
    }  

  temperaturePage.on([](AutoConnectAux& aux, PageArgument& args) {
    String tempStr = "Current Temperature: " + String(currentTemperature) + " °C";
    temperatureText.value = tempStr;
    aux["temp"].as<AutoConnectText>().value = tempStr;
    return String();
  }, AC_EXIT_AHEAD);
  portal.join({temperaturePage});
  String ssd = WiFi.SSID();
  if (ssd=="DIR-620") {
    Serial.println("DIR-6200 connect");
extern bool bIfLaser;
    bIfLaser = false;
  }


}

int setup_web_common_onconnect(void)
{ static int init = 0;

  Serial.print(F("WiFi connected, IP address: "));
  Serial.println(WiFi.localIP());
  sprintf(SmOT.LocalUrl,"http://%s", WiFi.localIP().toString().c_str());
  if(init)
    return 1;

   WiFi.setAutoReconnect(true);

/****************************************************/    
{

const char*  const _ntp1 = "europe.pool.ntp.org";
const char*  const _ntp2 = "pool.ntp.org";

    configTzTime("UTC0", _ntp1 ,_ntp2);

#if defined(ARDUINO_ARCH_ESP8266)
#elif defined(ARDUINO_ARCH_ESP32)
  Serial.print("Sync time in ms: ");
  Serial.println(sntp_get_sync_interval());  
#endif


#if MQTT_USE
   SmOT.Read_mqtt_fs();
#endif
}
/****************************************************/

  init  = 1;
  return 0;
}


extern bool bIfRelay;  
extern bool bIfEcMeter;
extern bool bIfPhMeter;
String on_Control(AutoConnectAux& aux, PageArgument& args) 
{ 
  if( CtrlChbUseRelay.checked) bIfRelay = true;
  else bIfRelay = false;
  if( CtrlChbUseEC.checked) bIfEcMeter = true;
  else bIfEcMeter = false;
  if( CtrlChbUsePH.checked) bIfPhMeter = true;
  else bIfPhMeter = false;

  int v = Set_DelayEC.value.toInt();
  if((unsigned int)v != EC_Meter.nDelayEC )
  { 
    EC_Meter.nDelayEC = v;
  }
  v = Set_CyclesEC.value.toInt();
  if((unsigned int)v != EC_Meter.nCyclesEC )
  { 
    EC_Meter.nCyclesEC = v;
  }

//    v = SetMQTT_port.value.toInt();
//    if((unsigned int)v !=SmOT.MQTT_port )
//    { isChangeMQTT++;
//       SmOT.MQTT_port = v;
//    }

  
/*
  if(bIfEcMeter)
  { if(SmOT.useMQTT == 0)
    { SmOT.useMQTT = 1;
      redir = 1;  
    } else if(SmOT.useMQTT == 1) { 
      SmOT.useMQTT = 0x3;
      isChangeMQTT++;
    }
  } else {
    if(SmOT.useMQTT != 0)
    { SmOT.useMQTT = 0;
      isChangeMQTT++;
    }
  }
*/
  char str[40];
  sprintf(str,"%.2f",Amt1001.T_Air);
  Set_Tair.value = str;
  sprintf(str,"%.2f",Amt1001.H_Air);
  Set_Hair.value = str;
  return String();
}

void onConnect(IPAddress& ipaddr) 
{ int rc;
  rc = setup_web_common_onconnect();
  if(rc)
  {
  }
}

// goes here from on_Setup
String onSetPar(AutoConnectAux& aux, PageArgument& args)
{  
  int isChange=0,  redir = 0, v;
  bool check;

#if MQTT_USE
  int isChangeMQTT = 0;
  if( CtrlChbUseMQTT.checked) check = true;
  else                  check = false;

  if(check)
  { if(SmOT.useMQTT == 0)
    { SmOT.useMQTT = 1;
      redir = 1;  
    } else if(SmOT.useMQTT == 1) { 
      SmOT.useMQTT = 0x3;
      isChangeMQTT++;
    }
  } else {
    if(SmOT.useMQTT != 0)
    { SmOT.useMQTT = 0;
      isChangeMQTT++;
    }
  }

   if(SmOT.useMQTT && redir== 0)
   {   char str0[80];
      int i;
 
    SetMQTT_server.value.toCharArray(str0, sizeof(str0));
    if(strcmp(SmOT.MQTT_server,str0))
    {  isChangeMQTT++;
       strcpy(SmOT.MQTT_server,str0);      
    }

    SetMQTT_user.value.toCharArray(str0, sizeof(str0));
    if(strcmp(SmOT.MQTT_user,str0))
    { isChangeMQTT++;
       strcpy(SmOT.MQTT_user,str0);      
    }
    SetMQTT_pwd.value.toCharArray(str0, sizeof(str0));
    if(strcmp(SmOT.MQTT_pwd,str0))
    { isChangeMQTT++;
       strcpy(SmOT.MQTT_pwd,str0);      
    }

    SetMQTT_devname.value.toCharArray(str0, sizeof(str0));
    if(strcmp(SmOT.MQTT_devname,str0))
    { isChangeMQTT++;
       strcpy(SmOT.MQTT_devname,str0);      
    }

    SetMQTT_topic.value.toCharArray(str0, sizeof(str0));
    /* check for [a-zA-Z0-9_-] */
    for(i=0; str0[i]; i++)
    {  if(str0[i]>='0' && str0[i]<='9' ) continue;
       if(str0[i]>='A' && str0[i]<='Z' ) continue;
       if(str0[i]>='a' && str0[i]<='z' ) continue;
       if(str0[i] =='_'  ) continue;
       if(str0[i] =='-'  ) continue;
       str0[i] = 0;
       break;
    }
    if(strcmp(SmOT.MQTT_topic,str0))
    { isChangeMQTT++;
       strcpy(SmOT.MQTT_topic,str0);      
    }

    v = SetMQTT_interval.value.toInt();
    if((unsigned int)v !=SmOT.MQTT_interval )
    { isChangeMQTT++;
       SmOT.MQTT_interval = v;
    }

    v = SetMQTT_port.value.toInt();
    if((unsigned int)v !=SmOT.MQTT_port )
    { isChangeMQTT++;
       SmOT.MQTT_port = v;
    }

   }

#endif //MQTT_USE

#if MQTT_USE
    if(isChangeMQTT)
    { 
      if(SmOT.useMQTT == 0x03) {
        mqtt_start();
      }
      SmOT.need_write_f |= 0x2;  //need write changes to FS
    }
#endif //MQTT_USE

  if(redir)
    aux.redirect(SETUP_URI);
  else
    aux.redirect(INFO_URI);

  return String();
}

// Redirects from root to the info page.
void onRoot() {
  WiFiWebServer&  webServer = portal.host();
  webServer.sendHeader("Location", String("http://") + webServer.client().localIP().toString() + String(INFO_URI));
  webServer.send(302, "text/plain", "");
  webServer.client().flush();
  webServer.client().stop();
}

float mRSSi = 0.;
int WiFists = -1;

int OutUTCtime(time_t now);

#include "esp32/rom/rtc.h"

// Main info page
String onInfo(AutoConnectAux& aux, PageArgument& args) {

  char str0[80];

/***************************************/
#if MQTT_USE 
if(SmOT.useMQTT)
{  extern int statemqtt;
   extern int state_mqtt;
   Info1.value += "<br>";

  switch(statemqtt)
  {   case -1:
        Info1.value += "MQTT not connected";
        if(SmOT.stsMQTT == 0)
            Info1.value += ", ожидание опроса OT";
        break;
      case 0:
        Info1.value += "MQTT DiSconnected";
        break;
      case 1:
        Info1.value += "MQTT connected";
        break;
  }
  
  switch(state_mqtt)
  {  
      case -1:
        Info1.value += " disconnected";
        break;

      case -2:
        Info1.value += " Connect failed";
        break;

      case -3:
        Info1.value += " Connection lost";
        break;
      case -4:
        Info1.value += " Connection timeout";
        break;
      case 1:
        Info1.value += " Bad protocol";
        break;

      case 2:
        Info1.value += " Bad client id";
        break;

      case 3:
        Info1.value += " unavailable";
        break;

      case 4:
        Info1.value += " Bad credentials";
        break;

      case 5:
        Info1.value += " Unauthorized";
        break;
  }
  
} else {
}
#else 
   if(SmOT.CapabilitiesDetected == 0)
          Info1.value += "<br>Тест котла";
#endif // MQTT_USE 
#if PID_USE
#endif // PID_USE 

        Info3.value = "";


      Info6.value = "";
    Info7.value = "";

/******************************/  
        SetNewBoilerTemp.enable = false;
        Info2.value = "";
        Info4.value = "";
        Info5.value = "";
        Info6.value = "";
        Info7.value = "";
/********************/
  return String();
}

String on_Setup(AutoConnectAux& aux, PageArgument& args)
{  const char *pstr; 
   char str[40]; 
/*********************************/      
    Info1.value ="";
    Ctrl2.value = ""; 
 
#if MQTT_USE
  CtrlChbUseMQTT.enable  = true;
  if(SmOT.useMQTT) 
  { if(SmOT.useMQTT == 1) 
      Info1.value = "проверь после Reset"; 
    CtrlChbUseMQTT.checked = true;
    SetMQTT_server.enable  = true;
    SetMQTT_user.enable  = true;
    SetMQTT_pwd.enable  = true;
    SetMQTT_topic.enable  = true;
    SetMQTT_interval.enable  = true;
    SetMQTT_devname.enable  = true;
    SetMQTT_port.enable  = true;

     SetMQTT_user.value = SmOT.MQTT_user;
     SetMQTT_pwd.value = SmOT.MQTT_pwd;

      SetMQTT_server.value = SmOT.MQTT_server;
      SetMQTT_topic.value = SmOT.MQTT_topic;
      sprintf(str, "%d",SmOT.MQTT_interval);
      SetMQTT_interval.value = str; 
      sprintf(str, "%d",SmOT.MQTT_port);
      SetMQTT_port.value = str; 

      SetMQTT_devname.value = SmOT.MQTT_devname;
    
  } else {
    CtrlChbUseMQTT.checked = false;
    SetMQTT_server.enable  = false;
    SetMQTT_user.enable  = false;
    SetMQTT_pwd.enable  = false;
    SetMQTT_topic.enable  = false;
    SetMQTT_interval.enable  = false;
    SetMQTT_devname.enable  = false;
    SetMQTT_port.enable  = false;
  }
#else //MQTT_USE

#endif //MQTT_USE

  return String();
}

const char SM_OT_HomePage[]= "https://t.me/smartTherm";
//"https://www.umkikit.ru/index.php?route=product/product&path=67&product_id=103";

int sRSSI = 0;
int razRSSI = 0;
extern int LedSts; 

void loop_web()
{  int rc,  dt;
static unsigned long t0=0, raz = 0; // t1=0;

  portal.handleClient();

  rc = WiFi.status();
  // ---- ниже можно удалить и будет работать
  { 
    static int oldstatus=-1, oldmode=-1, needStopAP=0 ;
    static long t0 = 0;
    int mode = WiFi.getMode();
    int ch = WiFi.channel();

    if((rc != oldstatus) || mode != oldmode)
    {
        if(rc == WL_CONNECTED &&  (oldstatus == WL_IDLE_STATUS || oldstatus == WL_DISCONNECTED ||  oldstatus == WL_NO_SSID_AVAIL))
        {   
          Serial.printf("WiFi status chage to connected");
          needStopAP = 1;
            t0 = millis();
        }
        oldmode = mode;
        oldstatus = rc;
    } else if(needStopAP) {
      if(millis()-t0 > 10000)
      {   needStopAP = 0;
          if(mode == WIFI_MODE_APSTA)  
          {  WiFi.softAPdisconnect(true);
            WiFi.enableAP(false);
          }
      }
    }
  }
  if(rc != WiFists) { 
    if(rc == WL_CONNECTED)
    {  LedSts = 0;
    } else {
      Serial.printf("WiFi disconnected (sts=%d)\n", rc);
      LedSts = 1;
    }

    if( rc >=0 && rc <=7)
        WiFiDebugInfo[rc]++;
    WiFists = rc;
  }


  if(rc ==  WL_CONNECTED)
  {  dt = millis() - t0;
     if(dt > 10000)
     {   t0 = millis();
         razRSSI++;
         sRSSI += WiFi.RSSI();
         if(razRSSI > 6*10)
         {  mRSSi =  float(sRSSI)/float(razRSSI);
            razRSSI = 0;
            sRSSI = 0;
         }
    }
  }
// ---- выше можно удалить и будет работать
    
#if MQTT_USE
  if(rc ==  WL_CONNECTED && (SmOT.useMQTT== 0x03))
         mqtt_loop();
#endif
  char str[40];
  sprintf(str,"%.2f",Amt1001.T_Air);
  Set_Tair.value = str;
  sprintf(str,"%.2f",Amt1001.H_Air);
  Set_Hair.value = str;

}

/**
 *  Convert dBm to the wifi signal quality.
 *  @param  rssi  dBm.
 *  @return A signal quality percentage.
 */
unsigned int /* AutoConnect:: */ _toWiFiQuality(int32_t rssi) {
  unsigned int  qu;
  if (rssi == 31)   // WiFi signal is weak and RSSI value is unreliable.
    qu = 0;
  else if (rssi <= -100)
    qu = 0;
  else if (rssi >= -50)
    qu = 100;
  else
    qu = 2 * (rssi + 100);

  // Здесь обновляем значение температуры (пример)
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {  // Обновляем каждые 2 секунды
    lastUpdate = millis();
    currentTemperature = Amt1001.T_Air;  // Ваша функция для чтения температуры
  }

  return qu;
}

int OutUTCtime(time_t now)
{   char str[312];
    char buffer[26];
    struct tm* tm_info;
    const char *s0 = "<em id=\"utcl\"></em><time id=\"upd_at\" dt=\"";
    const char *s1 = "\"></time><script>";
    const char *s2 =
"const src_el=document.getElementById('upd_at');\
const d=new Date(src_el.getAttribute('dt')).toLocaleString();\
document.getElementById(\"utcl\").innerHTML=d;</script>";
  char str2[40];
  sprintf(str2,"%.2f",Amt1001.T_Air);
  Set_Tair.value = str;
  sprintf(str2,"%.2f",Amt1001.H_Air);
  Set_Hair.value = str;

  tm_info = localtime(&now);

  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);  
  buffer[25] = 0;
  sprintf(str,"%s%sZ%s%s", s0,buffer,s1, s2);
  utc_time_jc = str;
  return 0;
}
/************************************/
void setup_read_config(void)
{ bool b;
  b = FlashFS.begin(AUTOCONNECT_FS_INITIALIZATION);
  if(b == false)
  {   Serial.println(F("FlashFS.begin failed"));
  }

  CtrlChbUseRelay.checked = bIfRelay;
  CtrlChbUseEC.checked = bIfEcMeter;
  CtrlChbUsePH.checked = bIfPhMeter;
  
}

void check_fs(void)
{ bool b;
#if defined(ARDUINO_ARCH_ESP32)
{ File root = FlashFS.open("/");
  File file = root.openNextFile();
 
  while(file){
 
#if SERIAL_DEBUG      
      Serial.print("FILE: ");
      Serial.printf( "%s %d\n", file.name(), file.size());
#endif      
      if(file.size() > 1000000)
       { char str[80];
         sprintf(str,"/%s",file.name() );
      #if SERIAL_DEBUG      
         Serial.printf( "remove %s\n", str);
      #endif         
         file.close();
         b = FlashFS.remove(str);
      #if SERIAL_DEBUG      
         Serial.printf( "remove  rc = %d\n", b);
      #endif         
         break;
       }
      
      file = root.openNextFile();
      
  }
}
#endif

#if SERIAL_DEBUG      
    { int tBytes, uBytes; 
#if defined(ARDUINO_ARCH_ESP8266)
      FSInfo info;
      FlashFS.info(info);
      tBytes  = info.totalBytes;
      uBytes = info.usedBytes;
#else
      tBytes  = FlashFS.totalBytes();
      uBytes = FlashFS.usedBytes();
#endif      
      Serial.printf("FlashFS tBytes = %d used = %d\n", tBytes, uBytes);
    }
#endif //SERIAL_DEBUG     

}
