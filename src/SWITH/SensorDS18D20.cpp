#include "Config.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include "SensorDS18D20.h"

SensorDS18D20::SensorDS18D20(String strN):LFsaAppl(strN) {
    // Топики для публикации состояния входов управления
//    mqttTopicDS18B20 = "demo/DS18B20/temperature=";
}

void SensorDS18D20::run() { ReadTemperatureDS18dD20(); }

// сигнальный провод датчика  на пин 13
#define ONE_WIRE_BUS 13
//extern PubSubClient client;
// создаём объект для работы с библиотекой OneWire
OneWire oneWireDS18B20(ONE_WIRE_BUS);

// создадим объект для работы с библиотекой DallasTemperature
DallasTemperature sensor(&oneWireDS18B20);

float SensorDS18D20::ReadTemperatureDS18dD20()
{
    // отправляем запрос на измерение температуры
    sensor.requestTemperatures();
    // считываем данные из регистра датчика
    sensDS18B20 = sensor.getTempCByIndex(0);
//    DS18B20=335.00;
    // выводим температуру в Serial-порт
    #if defined (MY_DEBUG)
    Serial.print("Temp(DS18B20) C: ");
    Serial.println(DS18B20);
    #endif
    // Публикуем данные на сервере
//    String str_temp(DS18B20);
//    client.publish(mqttTopicDS18B20, str_temp.c_str(), mqttSensorsRetained);
    return DS18B20;
}
