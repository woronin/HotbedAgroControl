/* SD_OpenTherm.cpp */
#include <time.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp32/rom/rtc.h"
typedef WebServer WEBServer;
#include <AutoConnect.h>
#include <AutoConnectFS.h>
extern AutoConnectFS::FS& FlashFS;

#include "SD_OpenTherm.hpp"

extern U8 *esp_get_buf (U16 size);
extern U8 *server_get_buf (U16 size);

int indcmd = 0;

/*******************************/
const int FS_BUF = 0; 

#if RELAY_USE    
                sizeof(SD_Termo::Relay_present) +  sizeof(SD_Termo::Relay_init_sts) +
#endif

#if MQTT_USE
const int FS_BUFMQTT =     
            sizeof(SD_Termo::useMQTT) + sizeof(SD_Termo::MQTT_server) + sizeof(SD_Termo::MQTT_user) + sizeof(SD_Termo::MQTT_pwd) + sizeof(SD_Termo::MQTT_topic) +
            sizeof(SD_Termo::MQTT_devname) + sizeof(SD_Termo::MQTT_interval) + sizeof(SD_Termo::MQTT_port);
#endif

/**^^^******************************/

const char *path="/smot_par";
const char *pathmqtt="/smotmqtt";

int SD_Termo::Read_data_fs(char *_path, uint8_t *dataBuff, int len, int &rlen, int mode)
{   int  n, nw, i, l;
    unsigned short int crs, crs_r, nn, v;

    rlen = 0;
#if SERIAL_DEBUG      
    Serial.printf((PGM_P)F("Reading file: %s\n"), _path);
#endif

    File file = FlashFS.open(_path,"r" );
    if(!file || file.isDirectory())
    {  
#if SERIAL_DEBUG      
         if(!file)
                Serial.println(F("- failed to open file for reading"));
        else
                Serial.println(F("- file.isDirectory"));
#endif                
        if(file)
        {   file.close();
#if SERIAL_DEBUG      
            Serial.println(F("file.close()"));
#endif            
        }

        return 1;
    }

/* *config version & length control */
    n = file.read((unsigned char *)&v, sizeof(nn));
   
    if(v != CONFIG_VERSION)
    {   file.close();
        return 2;
    }

    n = file.read((unsigned char *)&v, sizeof(nn));
    if(((mode == 0) && (v != FS_BUF)) ||((mode == 1) && (v != FS_BUFMQTT)) ) 
    {   file.close();
        return 3;
    }

//read 2 byte - length of data
    n = file.read((unsigned char *)&nn, sizeof(nn));
    if(n != sizeof(nn))
    {   file.close();
#if SERIAL_DEBUG      
        Serial.printf((PGM_P)F("file.read rc %i, must be =%i\n"),n,sizeof(nn));
#endif        
        return 4;
    }
    if(nn  > len)
    {
        Serial.printf((PGM_P)F("read file Buff size  %d, must be =%d\n"), len, nn);
        return 10;
    }

    crs = nn;
    nw = nn;
    n = file.read((unsigned char *) dataBuff, nw); //read nn bytes of data
    if(n != nw)
    {   file.close();
#if SERIAL_DEBUG      
        Serial.printf((PGM_P)F("file.read rc %i, must be =%i\n"),n,nw);
#endif        
        return 5;
    }
    l = n;
    n = file.read((unsigned char *)&crs_r, sizeof(short int));  //read 2 bytes control sum

    for(i=0; i<l; i++)
    {  crs += dataBuff[i];
    }

    if(crs !=  crs_r )
    {   file.close();
#if SERIAL_DEBUG      
        Serial.printf((PGM_P)F("crs = %i, must be =%i\n"),crs_r,crs);
#endif        
        return 6;
    }

    file.close();
#if SERIAL_DEBUG      
    Serial.println(F("file.close()"));
#endif
    rlen = l;
  
    return 0;
}


int SD_Termo::Write_data_fs(char *_path, uint8_t *dataBuff, int len, int mode)
{   int rc=0, i, n, nw;
    unsigned short int crs, v;

#if SERIAL_DEBUG      
    Serial.printf((PGM_P)F("Writing file: %s %d bytes\n"), _path, len);
#endif // SERIAL_DEBUG      

    File file = FlashFS.open(_path, "w");  //FILE_WRITE
    if(!file)
    {  
#if SERIAL_DEBUG      
         Serial.println(F("- failed to open file for writing"));
#endif         
        return 1;
    }
    v = CONFIG_VERSION;
    nw = file.write((unsigned char *) &v, sizeof(unsigned short int));
    v = FS_BUF;
    if(mode == 1)
        v = FS_BUFMQTT;
    n = file.write((unsigned char *) &v, sizeof(unsigned short int));
    nw += n;

    crs = (unsigned short int) len;
    n = file.write((unsigned char *) &crs, sizeof(unsigned short int));
    nw += n;
    for(i=0; i<len; i++)
      crs += dataBuff[i];
    n = file.write((unsigned char *) &dataBuff[0], len);
    if(n != len)
        rc = 1;
    else nw += n;    

    nw += file.write((unsigned char *) &crs, sizeof(unsigned short int));
    if(nw != (len + 4*sizeof(unsigned short int) ) )
        rc = 2;
    file.close();
    return rc;
}


int SD_Termo::Read_mqtt_fs(void)
{   int rc, n, nw;
    uint8_t Buff[FS_BUFMQTT];
    uint8_t len;

    rc = Read_data_fs((char *)pathmqtt, Buff, FS_BUFMQTT, nw, 1);
#if SERIAL_DEBUG      
    Serial.printf("Read %s rc %i\n", pathmqtt, rc);
#endif    
    if(rc)
        return 1;
#if MQTT_USE
    n = sizeof(useMQTT);
    memcpy((void *) &useMQTT, &Buff[0], sizeof(useMQTT));

    memcpy((void *) &len, &Buff[n], 1);  n++;
    memcpy((void *) MQTT_server, &Buff[n], len);  n += len;

    memcpy((void *) &len, &Buff[n], 1);  n++;
    memcpy((void *) MQTT_user, &Buff[n], len);  n += len;
    
    memcpy((void *) &len, &Buff[n], 1);  n++;
    memcpy((void *) MQTT_pwd, &Buff[n], len);  n += len;

    memcpy((void *) &len, &Buff[n], 1);  n++;
    memcpy((void *) MQTT_topic, &Buff[n], len);  n += len;

    memcpy((void *) &len, &Buff[n], 1);  n++;
    memcpy((void *) MQTT_devname, &Buff[n], len);  n += len;

    memcpy((void *) &MQTT_interval, &Buff[n], sizeof(MQTT_interval));
    n += sizeof(MQTT_interval);
    {   unsigned short _port;
        memcpy((void *) &_port, &Buff[n], sizeof(MQTT_port));
        MQTT_port = _port;
        n += sizeof(MQTT_port);
    }
#endif
    return 0;
}

int SD_Termo::Write_mqtt_fs(void)
{   int rc, n=0;
    uint8_t Buff[FS_BUFMQTT];
    uint8_t len;

#if MQTT_USE
    memcpy(&Buff[n],(void *) &useMQTT, sizeof(useMQTT));
    n += sizeof(useMQTT);

    len = strlen(MQTT_server)+1;
    memcpy(&Buff[n],(void *) &len, 1); n++;
    memcpy(&Buff[n],(void *) MQTT_server, len); n += len;

    len = strlen(MQTT_user)+1;
    memcpy(&Buff[n],(void *) &len, 1);  n++;
    memcpy(&Buff[n],(void *) MQTT_user, len);   n += len;

    len = strlen(MQTT_pwd)+1; 
    memcpy(&Buff[n],(void *) &len, 1); n++;
    memcpy(&Buff[n],(void *) MQTT_pwd, len);    n += len;

    len = strlen(MQTT_topic)+1; 
    memcpy(&Buff[n],(void *) &len, 1); n++;
    memcpy(&Buff[n],(void *) MQTT_topic, len);  n += len;

    len = strlen(MQTT_devname)+1; 
    memcpy(&Buff[n],(void *) &len, 1); n++;
    memcpy(&Buff[n],(void *) MQTT_devname, len);    n += len;

    memcpy(&Buff[n],(void *) &MQTT_interval, sizeof(MQTT_interval));
    n += sizeof(MQTT_interval);
    memcpy(&Buff[n],(void *) &MQTT_port, sizeof(MQTT_port));
    n += sizeof(MQTT_port);

#endif

    rc = Write_data_fs((char *)pathmqtt, Buff, n, 1);
#if SERIAL_DEBUG      
    Serial.printf("Write %s rc %i\n", pathmqtt, rc);
#endif

    return rc;
}

void SD_Termo::loop(void)
{   int dt;
    extern int WiFists;

    if(need_write_f)
    {   
        if(need_write_f & 0x01)
        {   
            Write_ot_fs();
            need_write_f &= ~0x01;
        }
        if(need_write_f & 0x02)
        {   
            Write_mqtt_fs();
            need_write_f &= ~0x02;
        }
     
    } else  if(WiFists  == WL_CONNECTED) {
    
    }
}

int SD_Termo::Write_ot_fs(void)
{   
    int rc, n;
    uint8_t Buff[FS_BUF];
    
    rc = Write_data_fs((char *)path, Buff, n, 0);

    return rc;
}
