#include "Config.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include "SensorDS18D20.h"

SensorDS18D20::SensorDS18D20(String strN):LFsaAppl(strN) {
}

void SensorDS18D20::run() { y1(); }

#define ONE_WIRE_BUS 13
OneWire oneWireDS18B20(ONE_WIRE_BUS);

DallasTemperature sensor(&oneWireDS18B20);

void SensorDS18D20::y1()
{
    // отправляем запрос на измерение температуры
    sensor.requestTemperatures();
    // считываем данные из регистра датчика
    sensDS18B20 = sensor.getTempCByIndex(0);
    // выводим температуру в Serial-порт
    #if defined (MY_DEBUG)
    Serial.print("Temp(DS18B20) C: ");
    Serial.println(DS18B20);
    #endif
}
