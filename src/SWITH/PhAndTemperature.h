#ifndef PHANDTEMP_H
#define PHANDTEMP_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <LFsaAppl.h>
class PhAndTemperature:public LFsaAppl 
{
public:
    void readpHValue();
    void readpHValue_fsa();
    void readTemperaturePH();
    PhAndTemperature(String strN);
    ~PhAndTemperature() {};
    void run();
    float PH_acc;
    float PH;
    float T_ph;
    float calibration{12.00}; //change this value to calibrate
protected:    
    int x1(); int x2();
    void y1(); void y2(); void y3(); void y4(); void y5();
    void y6(); void y7(); void y8(); void y9(); void y10();
    bool  mqttSensorsRetained{false}; 
    // Определяем пин, к которому подключен аналоговый выход датчика pH-4502C
    // Переменная для хранения значения pH
//    const char* mqttTopicPH;
//    const char* mqttTopicPhTemp;
    // Функция для калибровки и преобразования аналогового значения в pH
    //const int analogInPin = A0;
    const int analogInPin{35};
    unsigned long int avgValue;
    float b;
    int buf[10], temp;

    int i{0};
    int lastTime30{0};
    int lastTime500{0};
};
#endif