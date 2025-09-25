#ifndef ECMETER_H
#define ECMETER_H
//#define IF_KOTSUBA_EC
#include "Config.h"
#if defined(IF_EC_2DELAY)
#include <Arduino.h>
#include <LFsaAppl.h>
#include "ArithmeticMean.h"
#include "esp_adc_cal.h"
#define DEFAULT_VREF 1100

#if defined(IF_KOTSUBA_EC)     
static int NumRead{64};
#endif    

class EC_meter:public LFsaAppl 
{
public:
    void cond_sol_fsa();
    void ECMeterControl();
    EC_meter(String strN);
    ~EC_meter() {};
    void run();
    float EC_acc;
    float EC;
    bool EC1State, EC2State;
    int nDelayEC{0};
    long nCyclesEC{16};
    float fBasys{120};
    float fDegree{-2.6};
#if defined(IF_KOTSUBA_EC)

    const float EcK = 1.27; //китайский датчик  = 0.66 (вилка 6А) = 1.7/1.4 (вилка 4А)
    const float R_Divider = 5.; //делитель 5 кОм

    struct Ec_meter_measure
    { //unsigned long t;
    unsigned short a{0};
    unsigned short b{0};
    };

    int get_adc1(int adc_chan);
    int Get_adc1_mv(int reading);

    int Ec_meter_raw(int nd, int delay, double &dDap, double &dDan);
    int get_Ec(float K, float R0, float &R, float &Ec);
    int sts{0};
    int rc;
    float R, Ec;
    Ec_meter_measure buf0[64];
    int nread=0;
#endif    
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
    int x1();
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
    // Полученное с MQTT ("новое") состояние EС_meter
    char ECCommand1{lvlEcOff};
    char ECCommand2{lvlEcOff};

    const int sensEC{36};  // GPIO36 (дискрeтный вход) EС_meter
    const int sensEC1{22};  // GPIO22 (дискретный вход) EС_meter
    const int sensEC2{23};  // GPIO23 (дискрeтный вход) EС_meter

    int d1{23}, d2{22}, a1{36}; long var{1};
    unsigned long ap{0}, an{0};
    bool  mqttSensorsRetained{false}; 
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
