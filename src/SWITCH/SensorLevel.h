#ifndef SENSORLEVEL_H
#define SENSORLEVEL_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <LFsaAppl.h>
class SensorLevel:public LFsaAppl 
{
public:
    SensorLevel(String strN);
    ~SensorLevel() {};
    void run();
    void y1();
    float fLevel{0};
protected:
    bool levelStatus1{false};
    bool levelStatus2{false};
    bool levelStatus3{false};
    // Номера выводов для подключения реле
    const int gpioLevel1{18};
    const int gpioLevel2{19};
    const int gpioLevel3{21};
};
#endif