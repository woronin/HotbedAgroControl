#include "Config.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include "PhAndTemperature.h"
//extern PubSubClient client;
 
PhAndTemperature::PhAndTemperature(String strN):LFsaAppl(strN) {
//    mqttTopicPH              = "demo/pH-4502C/ph";
//    mqttTopicPhTemp          = "demo/pH-temp/pH-temperature";
    // Функция для калибровки и преобразования аналогового значения в pH
    //calibration = 0.00; //change this value to calibrate
    //const int analogInPin = A0;
}

void PhAndTemperature::run() { 
//    readpHValue();
    readpHValue_fsa();
  #if defined(DEBUG_PH)
  Serial.printf("PhAndTemperature: PH_acc:(%.2f)\n", PH_acc);
  Serial.printf("PhAndTemperature(State): %d\n", nState);
  #endif
}
int PhAndTemperature::x1() { return i<10; }
int PhAndTemperature::x2() { return (i<8); }
void PhAndTemperature::y1() { i=0; }; 
void PhAndTemperature::y2() { i++; }; 
void PhAndTemperature::y3() { buf[i] = analogRead(analogInPin); } 
void PhAndTemperature::y4() { } 
void PhAndTemperature::y5() {
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
}
void PhAndTemperature::y6() { avgValue = 0; } 
void PhAndTemperature::y7() {
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
} 
void PhAndTemperature::y8() {
  float pHVol = (float)avgValue * 3.3 / 4095.0 / 6; // Такие значения для нашего АЦП ESP32
  PH_acc = -5.70 * pHVol + calibration;
  // Публикуем данные на сервере
//  String str_ph(PH);
//  client.publish(mqttTopicPH, str_ph.c_str(), mqttSensorsRetained);
} 
void PhAndTemperature::y9() { delay(500); } 
void PhAndTemperature::y10() {
  readTemperaturePH();
#if defined (MY_DEBUG)
  Serial.print("pH Value: ");
  Serial.println(PH);
#endif
}

void PhAndTemperature::readpHValue_fsa() 
{
//  Serial.println();
//  Serial.print("nState= "); Serial.println(nState);
//  Serial.print("pH Value: "); Serial.println(PH);
  if (nState==0) { y1(); nTmpState=1;} 
  if (nState==1 && x1()) { y2(); y3(); lastTime30 = millis(); nTmpState=2; } 
  if (nState==1 && !x1()) { y5(); nTmpState=3; } 
  if (nState==2) { 
    if ((millis() - lastTime30) >= 30) {
      nTmpState=1; 
    }
  }
  if (nState==3) { y6(); y7(); nTmpState=4; } 
  if (nState==4) { y8(); lastTime500 = millis(); nTmpState=5; }
  if (nState==5) { 
    if ((millis() - lastTime500) >= 500) {
      nTmpState=6; 
    }
  } 
  if (nState==6) { y10(); nTmpState=0; } 
  if (nState==33) { nTmpState=0; } 

  if (nTmpState != nState) {
    nState = nTmpState;
  }
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
  // Публикуем данные на сервере
//  String str_ph(PH);
//  client.publish(mqttTopicPH, str_ph.c_str(), mqttSensorsRetained);
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
  // Публикуем данные на сервере
//  String str_temp(T_ph);
//  client.publish(mqttTopicPhTemp, str_temp.c_str(), mqttSensorsRetained);
  #if defined (MY_DEBUG)
  // Вывод считанных данных в лог
    Serial.print(F("%  Temperature pH: "));
    Serial.print(t);
    Serial.println(F("°C "));
  #endif
}
