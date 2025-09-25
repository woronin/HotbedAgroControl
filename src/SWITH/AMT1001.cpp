#include <LFsaAppl.h>
#include <amt1001_ino.h>
#include <PubSubClient.h>
#include "AMT1001.h"
//extern PubSubClient client;
extern bool bIfLaser;

AMT1001::AMT1001(String strN):LFsaAppl(strN) {
    H_Air = T_Air = 0.0;
    T_AirMin = -10.0; T_AirMax = 50.0;
}

void AMT1001::run() { 
  readTemperatureAMT1001(); 
}

// Адрес датчика AMT1001 по умолчанию
//#define AMT1001_I2C_ADDRESS 0x32
// Чтение температуры и влажности AMT1001
void AMT1001::readTemperatureAMT1001()
{
  // Читаем данные с сенсора
    if (bIfLaser)
        readAMT1001DataV(T_Air, H_Air);
    else  
        readAMT1001DataL(T_Air, H_Air);
//  readAMT1001DataL(T_Air, H_Air);
//  readAMT1001DataK(T_Air, H_Air);
  #if defined(DEBUG_AMT1001)
  Serial.printf("AMT1001: T_air:(%.2f)", T_Air); Serial.printf("H_air:(%.2f)\n", H_Air);
  #endif
// Публикуем данные на сервере
//  String str_temp(T_air);
//  client.publish(mqttTopicTemperature, str_temp.c_str(), mqttSensorsRetained);
//  String str_humd(H_air);
//  client.publish(mqttTopicHumidity, str_humd.c_str(), mqttSensorsRetained);
}
// Функция для чтения данных с датчика
bool AMT1001::readAMT1001DataL(float &temperature, float &humidity) {
    float sss = temperature;
    float temp, hum;
//*
    temp = amt1001_gettemperature(t_air); //  вычисляем температуру в С
    temp += 10.0; //  вычисляем температуру в С
    if (temp>T_AirMin && temp<T_AirMax) { temperature = temp; }        // отсекаем 
    else { temperature = sss; }
//    temperature = -777.77;
//*/
/*
    // считаем попугаи У =(5-1,785* Х )/(1,785* Х59)*10
    t_air *=4.15;
    double Y = ((5-(1.785* t_air))/(1.785* t_air))*10;
    Serial.printf("L-Y=%.4f, t_air=%f\n", Y, t_air);
    //У =-23,336*LN(Х)+79,581
    temp =((-23.336)*log(Y))+79.581;
    temperature = temp;
*/    


    //double volt = (double)hum * (5.0 / 1023.0);
    double volt = (double)h_air * (3.3 / 4095.0);
    humidity = amt1001_gethumidity(volt); //  вычисляем влажность в %
// Преобразование сырых данных в температуру и влажность
//  temperature = (tem / 100.0) - 40.0;
//  humidity = hum / 100.0;
    return true;
}
    
bool AMT1001::readAMT1001DataV(float &temperature, float &humidity) {
    // Получаем аналоговое значение (для ESP32, 12-bit ADC, 0-4095)
    float sav = temperature;
    float temp, hum;

    float t_a = t_air * (3.3 / 4095.0); //Преобразуем значение от АЦП ESP32 в Вольты
    double Y = ((5-(1.785* t_a))/(1.785* t_a))*10; // пересчитываем вольты в КОмы, по формуле с коэфициентами от Сереги
//    Serial.printf(" V-Y=%.4f, t_air=%f\n", Y, t_a);

    temp =((-23.336)*log(Y))+79.581; // Пересчитываем КОм в градусы Цельсия по таблице из DataSheet на  AMT1001, натуральный логарифм взят из LibreOffice
    if (temp>T_AirMin && temp<T_AirMax) { temperature = temp - 2; }        // отсекаем лишнее
    else { temperature = sav; }
//    temperature = temp - 2; // Вносим поправочный коэффициент, согласно калибровке
 
    double volt = (double)h_air * (3.3 / 4095.0);
    humidity = amt1001_gethumidity(volt); //  вычисляем влажность в %
    return true;
}

float temp_ntc1(int ADC_v);
bool AMT1001::readAMT1001DataK(float &temperature, float &humidity) {
    // Читаем аналоговое значение (для ESP32, 12-bit ADC, 0-4095)
//    float t_air = analogRead(32);
//    float h_air = analogRead(33);
//    #if defined(DEBUG_AMT1001)
//    Serial.printf("K-Parrots(AMT1001):t_air(%.2f), h_air(%.2f)\n", t_air, h_air);
//    #endif
    temperature = temp_ntc1(t_air);
    double volt = (double)h_air * (3.3 / 4095.0);
    humidity = amt1001_gethumidity(volt); //  вычисляем влажность в %
  
    return true;
}

float temp_ntc1(int ADC_v)
{ 
  float resistorValue = 10000.;
  double R1 = 10000.0;   // voltage divider resistor value
  double Beta = 3950.0;  // Beta value
  double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
  double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius  
  double Vout, Rt = 0, R;
  double T, Tc, Tf = 0;

  // Convert analog reading to resistance
  Rt = R1 / ((4095.0 / ADC_v) - 1.0);
  //1/Rt = 1/R + 1/R0   R0 = 510000/;
  //1/R = 1/Rt - 1/R0
  R = 1./Rt - 1./510000.;
  R = 1 / R;
  // Use the Steinhart-Hart equation to convert resistance to temperature in Celsius
  T = 1/(1/To + log(R/Ro)/Beta);    // Temperature in Kelvin
  Tc = T - 273.15;                   // Celsius
  return Tc; 
}
