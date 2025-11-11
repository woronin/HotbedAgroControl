#include "Config.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include "PhAndTemperature.h"
 
#if defined (IF_EC_2DELAY)
#include "EC_meter2d.h"
extern EC_meter EC_Meter;
#endif
PhAndTemperature::PhAndTemperature(String strN):LFsaAppl(strN) { }

void PhAndTemperature::run() { fsa(); }
// предикаты
int PhAndTemperature::x1() { return i<10; }
int PhAndTemperature::x2() { return (i<8); }
int PhAndTemperature::x3() { return (millis() - lastTime30) >= 30; }
int PhAndTemperature::x4() { return (millis() - lastTime30) >= 500; }
int PhAndTemperature::x5() { 
#if defined (IF_EC_2DELAY)
  return EC_Meter.FGetState() == "0"; 
#endif  
  return true; 
}
// действия
void PhAndTemperature::y1() { i=0; }; 
void PhAndTemperature::y2() { i++; }; 
void PhAndTemperature::y3() { buf[i] = analogRead(analogInPin); } 
void PhAndTemperature::y5() {
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) { 
        temp = buf[i]; buf[i] = buf[j]; buf[j] = temp; 
      }
    }
  }
}
void PhAndTemperature::y6() { avgValue = 0; } 
void PhAndTemperature::y7() {
  for (int i = 2; i < 8; i++) avgValue += buf[i];
} 
void PhAndTemperature::y8() {
  float pHVol = (float)avgValue * 3.3/4095.0/6;
  PH_acc = -5.70 * pHVol + calibration;
} 
void PhAndTemperature::y10() { readTemperaturePH();
  #if defined (MY_DEBUG) 
    Serial.print("pH Value: "); Serial.println(PH);
  #endif
}
void PhAndTemperature::y11() { lastTime30 = millis(); }
void PhAndTemperature::y12() { lastTime500 = millis(); }

void PhAndTemperature::fsa() 
{
  if (nState==0 && x5()) { y1(); nTmpState=1;} 
  if (nState==1 && x1()) { y2(); y3(); y11(); nTmpState=2; } 
  if (nState==1 && !x1()) { y5(); y6(); y7(); y8(); y12(); nTmpState=5; } 
  if (nState==2 && x3()) { nTmpState=1; }
  if (nState==5 && x4()) { nTmpState=6; } 
  if (nState==6) { y10(); nTmpState=0; } 

  if (nTmpState != nState) { nState = nTmpState; }
}
//  
void PhAndTemperature::readpHValue() {
  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(analogInPin);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  float pHVol = (float)avgValue * 3.3 / 4095.0 / 6; // Такие значения для нашего АЦП ESP32
  PH_acc = -5.70 * pHVol + calibration;
  delay(500);
  readTemperaturePH();
  #if defined (MY_DEBUG)
  Serial.print("pH Value: ");
  Serial.println(PH);
  #endif
}
// Чтение температуры и влажности PH-4502C
void PhAndTemperature::readTemperaturePH()
{
// Функция для чтения данных с датчика температуры pH
  float tem = analogRead(34);
  double volt = (double)tem * (3.3 / 4095.0);
  volt += 18; //  вычисляем температуру в С
  T_ph = volt; //  вычисляем температуру в С
  #if defined (MY_DEBUG)
  // Вывод считанных данных в лог
    Serial.print(F("%  Temperature pH: "));
    Serial.print(t);
    Serial.println(F("°C "));
  #endif
}
