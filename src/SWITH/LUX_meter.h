#ifndef LUXMETER_H
#define LUXMETER_H
#include <Arduino.h>
#include <PubSubClient.h>
#include <LFsaAppl.h>
class LUX_meter:public LFsaAppl 
{
public:
    void readDigitalInputsLux();
    float readLuxValueNew();        // почта: 30/06/2025_1554
    float readLuxValue();
    void LUXMeterControl();
    LUX_meter(String strN);
    ~LUX_meter() {};
    void run();
    void y1();
    float L, LUX;
    // Текущее состояние входов управления
    bool FStatus1;
    bool FStatus2;

protected:    
    // Текстовое отображение для состояния входов
    const char* mqttLuxStatusOn2;
    const char* mqttLuxStatusOff2;
    const bool  mqttLuxStatusRetained{false}; 
    char lvlFOn{0x1};
    char lvlFOff{0x0};
    // Полученное с MQTT ("новое") состояние LUX_meter
    bool FCommand1;
    bool FCommand2;
    // Определяем пин, к которому подключен аналоговый выход датчика LUX-meter
    int sensLUX{39};  // GPIO35 (дискрeтный вход) LUX_meter

    const int gpioF1{12};
    const int gpioF2{14};
    char inputStatusF1{2};
    char inputStatusF2{2};
    const bool  mqttSensorsRetained{false}; 
};
#endif  //LUXMETER