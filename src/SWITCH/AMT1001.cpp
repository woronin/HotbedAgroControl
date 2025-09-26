#include <LFsaAppl.h>
#include <amt1001_ino.h>
#include <PubSubClient.h>
#include "AMT1001.h"
extern bool bIfLaser;

AMT1001::AMT1001(String strN):LFsaAppl(strN) { }

void AMT1001::run() { y1(); }
// Чтение температуры и влажности AMT1001
void AMT1001::y1()
{
  // Читаем данные с сенсора
    if (bIfLaser)
        readAMT1001DataV(T_Air, H_Air);
    else  
        readAMT1001DataL(T_Air, H_Air);
}
// Функция для чтения данных с датчика
bool AMT1001::readAMT1001DataL(float &temperature, float &humidity) {
    float sss = temperature;
    float temp, hum;
    temp = amt1001_gettemperature(t_air); //  вычисляем температуру в С
    temp += 10.0; //  вычисляем температуру в С
    if (temp>T_AirMin && temp<T_AirMax) { temperature = temp; }        // отсекаем 
    else { temperature = sss; }
    double volt = (double)h_air * (3.3 / 4095.0);
    humidity = amt1001_gethumidity(volt); //  вычисляем влажность в %
    return true;
}
    
bool AMT1001::readAMT1001DataV(float &temperature, float &humidity) {
    // Получаем аналоговое значение (для ESP32, 12-bit ADC, 0-4095)
    float sav = temperature;
    float temp, hum;

    float t_a = t_air * (3.3 / 4095.0); //Преобразуем значение от АЦП ESP32 в Вольты
    double Y = ((5-(1.785* t_a))/(1.785* t_a))*10; // пересчитываем вольты в КОмы, по формуле с коэфициентами от Сереги

    temp =((-23.336)*log(Y))+79.581; // Пересчитываем КОм в градусы Цельсия по таблице из DataSheet на  AMT1001, натуральный логарифм взят из LibreOffice
    if (temp>T_AirMin && temp<T_AirMax) { temperature = temp - 2; }        // отсекаем лишнее
    else { temperature = sav; }
 
    double volt = (double)h_air * (3.3 / 4095.0);
    humidity = amt1001_gethumidity(volt); //  вычисляем влажность в %
    return true;
}

