#ifndef ECMETER_H
#define ECMETER_H
#include "Config.h"
#if defined(IF_EC_2DELAY)
#include <Arduino.h>
#include <LFsaAppl.h>
#include "ArithmeticMean.h"
#include "esp_adc_cal.h"
#define DEFAULT_VREF 1100

class EC_meter:public LFsaAppl 
{
public:
    void ECMeterControl();
    EC_meter(String strN);
    ~EC_meter() {};
    void run();
    float EC_acc;
    float EC;
    bool EC1State, EC2State;
    int nDelayEC{0};
    long nCyclesEC{16};
    float fBasys{647.66};
    float fDegree{-2.54};
    String NumToWidth(int width);
    String NumToAtten(int atten);
    void check_efuse();
    void adc1_config(int adc_chan); 
    void print_char_val_type(esp_adc_cal_value_t val_type);
    void adc1_calibrate(int mode);
    void setup_Ec_meter(uint8_t _Ap_Pin, uint8_t _An_Pin, uint8_t _Adc1_Chan);
    esp_adc_cal_characteristics_t *adc_chars = NULL;
    adc1_channel_t Adc1_Chan = ADC1_CHANNEL_0;
    adc1_channel_t ADC_channel = ADC1_CHANNEL_0;
    uint8_t Ap_Pin = 22;
    uint8_t An_Pin = 23;

protected:
    int x1(); int x2(); int x5();
    void y1(); void y2(); void y3(); void y5(); void y6(); void y13();    
    void y14();
    const int gpioEC1{22};
    const int gpioEC2{23};

    const char lvlEcOn{0x1};
    const char lvlEcOff{0x0};
    // Текущее состояние реле
    char EcStatus1{lvlEcOff};
    char EcStatus2{lvlEcOff};
    // Полученное с MQTT ("новое") состояние EС_meter
    char ECCommand1{lvlEcOff};
    char ECCommand2{lvlEcOff};

    int d1{23}, d2{22}, a1{36}; long var{1};
    unsigned long ap{0}, an{0};
    CArrayFilter dArrayAp;
    CArrayFilter dArrayAn;
    int nN{50};
    double dDap;
    double dDATap;
    double dDan;
    double dDATan;

};
#endif
#endif