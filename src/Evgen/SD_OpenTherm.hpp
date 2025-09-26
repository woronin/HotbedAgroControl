/* SD_OpenTherm.hpp */
#ifndef SD_OPENTHERM
#define SD_OPENTHERM

#include "SmartDevice.hpp"

class SD_Termo:public SmartDevice
{
public:
  float dhw_t;   // DHW temperature (°C)
  byte need_write_f; 
  
#if MQTT_USE
  byte useMQTT;  //0 = not use, 1 use but not setup, 0x3 - use & setup
  byte stsMQTT;
 //40+40+4+40+20+40=  184 (+110)
  char MQTT_server[80];
  char MQTT_topic[40];
  int MQTT_interval; //sec
  char MQTT_user[40];
  char MQTT_pwd[20];
  char MQTT_devname[40];
  unsigned short MQTT_port;  /* MQTT port, default 1883 */
  int MQTT_need_report;
#endif //MQTT_USE
  int start_sts; //1 - start state, need ask server for last I and U0(?),  &0x02 - OT start log, 0 - not start

  SD_Termo(void)
  {	  

      dhw_t = 0.;
#if MQTT_USE
      useMQTT = 0;
      stsMQTT = 0;
      strcpy(MQTT_server,"80.237.133.118");
      strcpy(MQTT_topic,"HB");
      strcpy(MQTT_devname,"AgroControl");
      strcpy(MQTT_user,"mq_umki");
      strcpy(MQTT_pwd,"54321");
      MQTT_interval = 10; //sec
      MQTT_need_report = 0;
      MQTT_port = 12883;
#endif     
    start_sts = 1;
    need_write_f = 0;

  }
  void loop(void);
  int Write_data_fs(char *path, uint8_t *dataBuff, int len, int mode);
  int Read_data_fs(char *path, uint8_t *dataBuff, int len, int &rlen, int mode);
  int Read_mqtt_fs(void);
  int Write_mqtt_fs(void);
  int Write_ot_fs(void);

};

#endif // SD_OPENTHERM