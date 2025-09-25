#ifndef AMT1001_H
#define AMT1001_H
#include "Config.h"
#include <LFsaAppl.h>
class AMT1001:public LFsaAppl 
{
public:
    bool readAMT1001DataK(float &temperature, float &humidity);
    bool readAMT1001DataV(float &temperature, float &humidity);
    bool readAMT1001DataL(float &temperature, float &humidity);
    void readTemperatureAMT1001();
    AMT1001(String strN);
    ~AMT1001() {};
    void run();
    void y1();

//    float H, T;
    float H_Air{0}, T_Air{0};
    float T_AirMin{-10}, T_AirMax{50};
    float t_air{0};
    float h_air{0};
    float vt_air{0};
    float vh_air{0};
    int     pinT{32};
    int     pinH{33};
protected:    
    // Топики для публикации температуры и влажности
};
#endif