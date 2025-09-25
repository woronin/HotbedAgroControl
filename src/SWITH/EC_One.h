#ifndef EC_ONE_H
#define EC_ONE_H
#include "Config.h"
#if defined(IF_EC_ONE)
#include <Arduino.h>
#include <LFsaAppl.h>
#include "ArithmeticMean.h"
class EC_ONE:public LFsaAppl 
{
public:
    void cond_sol_fsa();
    void ECMeterControl();
    void readECValue();
    void readDigitalInputsEC();
    EC_ONE(String strN);
    ~EC_ONE() {};
    void run();
    float EC_acc;
    float EC;
    bool EC1State, EC2State;
    int nDelayEC{0};
    long nCyclesEC{500};

protected:
    void y1(); void y3(); void y4(); void y5(); void y6(); void y13();    
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

    int d1{22}, d2{23}, a1{36};
    unsigned long ap{0}, an{0};
    CArrayFilter dArrayAp;
    CArrayFilter dArrayAn;
    int nN{50};

};
#endif
#endif  // EC_ONE_H