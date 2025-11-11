#ifndef LUXMETER_H
#define LUXMETER_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <LFsaAppl.h>
class LUX_meter:public LFsaAppl 
{
public:
    void y1();        // почта: 30/06/2025_1554
    void LUXMeterControl();
    LUX_meter(String strN);
    ~LUX_meter() {};
    void run();
    float L, LUX;
    // Текущее состояние входов управления
    bool FStatus1;
    bool FStatus2;

protected:    
    // Полученное с MQTT ("новое") состояние LUX_meter
    bool FCommand1;
    bool FCommand2;
    // Определяем пин, к которому подключен аналоговый выход датчика LUX-meter
    int sensLUX{39};  // GPIO35 (дискрeтный вход) LUX_meter

    const int gpioF1{12};
    const int gpioF2{14};
    char inputStatusF1{2};
    char inputStatusF2{2};
};
#endif  //LUXMETER