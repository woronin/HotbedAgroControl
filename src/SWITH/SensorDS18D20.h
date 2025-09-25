#ifndef SENSORDS18D20_H
#define SENSORDS18D20_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <LFsaAppl.h>
class SensorDS18D20:public LFsaAppl 
{
public:
    float ReadTemperatureDS18dD20();
    SensorDS18D20(String strN);
    ~SensorDS18D20() {};
    void run();
    void y1();
    float sensDS18B20;
    float DS18B20;
protected:    
//    const char* mqttTopicDS18B20;
//    const bool  mqttSensorsRetained{false}; 
};
#endif