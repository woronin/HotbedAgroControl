#ifndef ECMETER_H
#define ECMETER_H
#include "Config.h"
#if defined(IF_EC)
#include <Arduino.h>
#include <LFsaAppl.h>
#include "ArithmeticMean.h"
class EC_meter:public LFsaAppl 
{
public:
    float cond_sol(int d1, int d2, int a0);
    void cond_sol_fsa();
    float fConductivity(int d1, int d2, int a1, long cnt);
    void ECMeterControl();
    void readECValue();
    void readDigitalInputsEC();
    EC_meter(String strN);
    ~EC_meter() {};
    void run();
    float EC_acc;
    float EC;
    bool EC1State, EC2State;
    int nDelayEC{500};
    long nCyclesEC{10};

protected:
    int x1();
    void y1(); void y2(); void y3(); void y4(); void y5(); void y6(); void y13();    
    void y14();
    const int gpioEC1{22};
    const int gpioEC2{23};

    char inputStatusEC1{2};
    char inputStatusEC2{2};

    const char lvlEcOn{0x1};
    const char lvlEcOff{0x0};
    // Текущее состояние реле
    char EcStatus1{lvlEcOff};
    char EcStatus2{lvlEcOff};
    // Полученное с MQTT ("новое") состояние ES_meter
    char ECCommand1{lvlEcOff};
    char ECCommand2{lvlEcOff};
// Определяем пин, к которому подключен аналоговый выход датчика pH-4502C
    const int sensEC{36};  // GPIO35 (дискрeтный вход) ES_meter
    const int sensEC1{22};  // GPIO22 (дискретный вход) ES_meter
    const int sensEC2{23};  // GPIO23 (дискрeтный вход) ES_meter

    int d1{23}, d2{22}, a1{36}; long var{1};
    unsigned long ap{0}, an{0};
    bool  mqttSensorsRetained{false}; 
    CArrayFilter dArrayAp;
    CArrayFilter dArrayAn;
    int nN{50};

};
#endif
#endif