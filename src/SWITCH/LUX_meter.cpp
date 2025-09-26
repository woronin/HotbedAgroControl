#include "Config.h"
#include "LUX_meter.h"
LUX_meter::LUX_meter(String strN):LFsaAppl(strN) {
    FStatus1 = false; FStatus2 = false;
    FCommand1 = true; FCommand2 = true;
}

void LUX_meter::run() { y1(); }
// Определяем пин, к которому подключен аналоговый выход датчика LUX-meter
const int sensLUX = 39;  // GPIO35 (дискрeтный вход) LUX_meter

void LUX_meter::y1() 
{

    float sensorValue = analogRead(sensLUX);  // Чтение аналогового значения
    float voltage = sensorValue * (3.3 / 4095.0);  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)
    int bDegree=1;   // степень - OFF(2) ON(4)
    //  OFF - яркая улица
    if(FCommand1 && FCommand2)  { bDegree = 4; }
    //  ON - темная комната
    if(FCommand1 && !FCommand2) { bDegree = 2; }
    float x = voltage; float Y;
    if (bDegree == 2) {
      // ступень 2  прямая
      Y  = 6053.183 * x - 4927.1739 ; // подобранны калибровочные коэф. Для 2-й ступени
      // ступень 2  степенная
      Y  = 1279.753 * pow(x, 2.377);
    }
    else if  (bDegree == 4) {
      // ступень 4  степенная
      Y  = 14936.076 * pow(x, 2.035);
    }
	  else {
	    Y=9999;
	  }
    L = Y;    
    // Вывод считанных данных в лог
    #if defined (MY_DEBUG)
      Serial.print("LUX Value: ");
      Serial.println(LUX);
    #endif
}
// Управление LUX_meter
void LUX_meter::LUXMeterControl()
{
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (FCommand1 != FStatus1) {
    FCommand1 = FStatus1;
    digitalWrite(gpioF1, FStatus1);
  };
  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (FCommand2 != FStatus2) {
    FCommand2 = FStatus2;
    digitalWrite(gpioF2, FStatus2);
  };
}
