//*
// Вариант Воронина на 10.07.2025
#include "Config.h"
#include "LUX_meter.h"
LUX_meter::LUX_meter(String strN):LFsaAppl(strN) {
    mqttLuxStatusOn2       = "on";
    mqttLuxStatusOff2      = "off";
    lvlFOn  = 0x1;
    lvlFOff = 0x0;
    FStatus1 = false;
    FStatus2 = false;
    FCommand1 = true;
    FCommand2 = true;

}

void LUX_meter::run() { readLuxValueNew(); }

//extern PubSubClient client;

// Контроль состояния входов
void LUX_meter::readDigitalInputsLux()
{
  // Читаем вход LUX_meter F1
  char gpioNewStatusF1 = digitalRead(gpioF1);
  if (gpioNewStatusF1 != inputStatusF1) {
    inputStatusF1 = gpioNewStatusF1;
  
//    client.publish(mqttTopicStatusInputF1, (inputStatusF1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow), mqttInputStatusRetained);

    #if defined (MY_DEBUG)
    Serial.print("Input LUX_F1 has changed its state: ");
    Serial.println(inputStatusF1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
    #endif
  };
  // Читаем вход LUX_meter F2
  char gpioNewStatusF2 = digitalRead(gpioF2);
  if (gpioNewStatusF2 != inputStatusF2) {
    inputStatusF2 = gpioNewStatusF2;
    #if defined (MY_DEBUG)
    Serial.print("Input LUX_F2 has changed its state: ");
    Serial.println(inputStatusF2 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
    #endif
  };
}

// Определяем пин, к которому подключен аналоговый выход датчика LUX-meter
const int sensLUX = 39;  // GPIO35 (дискрeтный вход) LUX_meter

const char* mqttTopicLUX              = "demo/LUX/lux";
// Функция для калибровки и преобразования аналогового значения в LUX
float LUX_meter::readLuxValue() 
{

    int sensorValue = analogRead(sensLUX);  // Чтение аналогового значения
    float voltage = sensorValue * (3.3 / 4095.0);  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)
    float offset = 0;  // Примерное значение смещения
    float scale = 1;   // Примерное значение масштаба
//    LUX = (voltage - offset) / scale;
    L = (voltage - offset) / scale;
  
    // Вывод считанных данных в лог
    #if defined (MY_DEBUG)
      Serial.print("LUX Value: ");
      Serial.println(LUX);
    #endif
  
    // Публикуем данные на сервере
    String str_es(LUX);
//    client.publish(mqttTopicLUX, str_es.c_str(), mqttSensorsRetained);
    #if defined(DEBUG_LUX)
    Serial.printf("Parrots(Lux_Meter):v(%.2f)", sensorValue);
    #endif

    return L;
}
// Функция для калибровки и преобразования аналогового значения в LUX (почта: 30/06/2025_1554)
//
//
//
// Для возведения числа в степень в C++ есть функция pow. Данная функция доступна при подключении <cmath> 
// (данный файл также объявляет такие функции как log, fabs и exp).
// По своей сути функция pow эквивалентна следующей формуле: pow(base, exponent) = baseexponent
// Данная функция объявлена следующим образом:
// double pow (double base, double exponent);
// Из данной сигнатуры видно, что:
// первым аргументом функция pow принимает основание, которое будем возводить в степень;
// вторым аргументом нужно передать степень, в которую нужно возвести основание;
// результат данной функции (основание возведенное в степень) имеет тип double.

// Для того, чтобы вычислить экспоненту числа в C/C++, мы можем воспользоваться встроенной функцией exp. 
// Эта функция доступна нам после подключения файла <math.h>. 
// В C++ можно подключить <cmath>. Вот как выглядит прототип этой функции:
// double exp (double x);
// Единственным аргументом функция принимает число, для которого нужно вычислить экспоненту.
// Функция возвращает экспоненту переданного ей аргумента.      

float LUX_meter::readLuxValueNew() 
{

    float sensorValue = analogRead(sensLUX);  // Чтение аналогового значения
    float voltage = sensorValue * (3.3 / 4095.0);  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)
    int bDegree=1;   // степень - OFF(2) ON(4)
    if(FCommand1 && FCommand2)  //  OFF - яркая улица
    { bDegree = 4; 
	}
    if(FCommand1 && !FCommand2) //  ON - темная комната
	{ bDegree = 2;  
	}

    float x = voltage; 
    float Y;
    if (bDegree == 2) {
      // ступень 2  прямая
      Y  = 6053.183 * x - 4927.1739 ; // подобранны калибровочные коэф. Для 2-й ступени
      // ступень 2  степенная
      // Y  = 1279.753 * x^ 2.377
      Y  = 1279.753 * pow(x, 2.377);
    }
    else if  (bDegree == 4) {
      // ступень 4  степенная
      Y  = 14936.076 * pow(x, 2.035);
      // ступень 4  экспоненциальная
      // Y  = 900.741 * exp(2.627 * x) ;
//      Y  = 900.741 * exp(x) ;
    }
	else 
	{
	Y=9999;
	}
    L = Y;    
    // Вывод считанных данных в лог
    #if defined (MY_DEBUG)
      Serial.print("LUX Value: ");
      Serial.println(LUX);
    #endif
    #if defined(DEBUG_LUX)
    Serial.printf("N-Parrots(Lux_Meter):v(%.2f)", sensorValue);
    #endif
    return L;
}
// Управление LUX_meter
void LUX_meter::LUXMeterControl()
{
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (FCommand1 != FStatus1) {
    FCommand1 = FStatus1;
    digitalWrite(gpioF1, FStatus1);
#if defined(DEBUG_LUX)    
    Serial.print("F1 has changed its state: ");
    Serial.println(FStatus1 == 1 ? mqttLuxStatusOn2 : mqttLuxStatusOff2);
#endif    
  };

  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (FCommand2 != FStatus2) {
    FCommand2 = FStatus2;
    digitalWrite(gpioF2, FStatus2);
#if defined(DEBUG_LUX)    
    Serial.print("F2 has changed its state: ");
    Serial.println(FStatus2 == 1 ? mqttLuxStatusOn2 : mqttLuxStatusOff2);
#endif    
  };
}
