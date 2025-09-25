#include "Config.h"
#if defined(IF_EC_ONE)
#include <Arduino.h>
#include <PubSubClient.h>
#include "EC_One.h"
//#define Voron
#define EC_VALUE 1000
EC_ONE::EC_ONE(String strN):LFsaAppl(strN) 
{ }

void EC_ONE::run() { 
cond_sol_fsa();
}

void EC_ONE::y1() {
  digitalWrite(d2, HIGH);
  digitalWrite(d1, LOW);
} 
void EC_ONE::y3() {
  double dDAT = analogRead(a1);
  Serial.printf("ONE:dDAT(ap)= %.4f\n", dDAT);
//  double dDAT = EC_VALUE;
#if not defined (Voron)  
  dArrayAp.Put(dDAT, nN, true);
  dDAT = dArrayAp.Get(nN, true);
#endif  
  ap = dDAT;
//  Serial.printf(".EC(dat)= %d\n", dat);
//  ap = 0 + dat + ap;
//  Serial.printf(".EC= %d\n", ap);
#if not defined (Voron)  
  digitalWrite(d2, LOW);
#endif  
  digitalWrite(d1, HIGH);
  digitalWrite(d2, LOW);
#if defined (DEBUG_EC)
  Serial.print("-");
#endif
}
void EC_ONE::y4() {} 
void EC_ONE::y5() {
  double dDAT = analogRead(a1);
  Serial.printf("ONE:dDAT(an)= %.4f\n", dDAT);
//  double dDAT = EC_VALUE;
#if not defined (Voron)  
  dArrayAn.Put(dDAT, nN, true);
  dDAT = dArrayAn.Get(nN, true);
#endif  
  an = 4095 - dDAT;
  digitalWrite(d1, LOW);
}
void EC_ONE::y6() {
  #if defined (MY_DEBUG)
    Serial.println("y6");
  #endif
  #if defined DEBUG_EC
    Serial.printf("nCycleEC = %d\n", nCyclesEC);
  #endif
  pinMode(d1, INPUT); pinMode(d2, INPUT);
// Усреднение АЦП при положительной ap и отрицательной an фазе
// Усреднение АЦП при положительной фазе
  float ap_raw=(float)ap;
// Усреднение АЦП при отрицательной фазе
  float an_raw=(float)an;
// Расчет поляризации раствора 
float po;
#if defined(IF_OM)
  if (an_raw>0)
    po = (ap_raw/an_raw)*10;
  else
    po = -1;
#else 
  po = ap_raw-an_raw;
#endif  
   Serial.printf("ap_raw=%f an_raw=%f po=%f\n", ap_raw, an_raw, po);
//  
#if defined(IF_OM)
  EC_acc = ((float)ap/(float)an)*10;
#else 
  EC_acc = ((float)ap + (float)an)/2;
#endif  

#if not defined (Voron)  
  #if defined DEBUG_EC
  Serial.printf("ap_raw=%f an_raw=%f po=%f\n", ap_raw, an_raw, po);
  #endif
#endif  
//  
#if not defined (Voron)  
  #if defined DEBUG_EC
  EC_acc = EC_acc * (3.3 / 4095.0) - 0.96;  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)
  #endif
#endif  
  // Вывод считанных данных в лог
  #if defined (DEBUG_EC)
    #if not defined (Voron)  
    Serial.print("EC Value: ");
    Serial.println(EC);
    #endif  
  #endif
}   

void EC_ONE::y13() { pinMode(d1, OUTPUT); pinMode(d2, OUTPUT); ap = 0, an = 0; }   

void EC_ONE::y14() { pinMode(d1, OUTPUT); pinMode(d2, OUTPUT); }   

// Контроль состояния входов
void EC_ONE::readDigitalInputsEC()
{
  // Читаем вход EC1
  char gpioNewStatusEC1 = digitalRead(gpioEC1);
  if (gpioNewStatusEC1 != inputStatusEC1) {
    inputStatusEC1 = gpioNewStatusEC1;
  
 //   Serial.println("Input EC1 has changed its state: ");
  };
  // Читаем вход EC2
  char gpioNewStatusEC2 = digitalRead(gpioEC2);
  if (gpioNewStatusEC2 != inputStatusEC2) {
    inputStatusEC2 = gpioNewStatusEC2;
  
 //   Serial.println("Input EC2 has changed its state: ");
  };
}

// Функция для калибровки и преобразования аналогового значения в pH
void EC_ONE::readECValue() {

}
// Управление ES_meter
void EC_ONE::ECMeterControl()
{
  ECCommand1 = digitalRead(gpioEC1);
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (ECCommand1 != EcStatus1) {
    EcStatus1 = ECCommand1;
    digitalWrite(gpioEC1, EcStatus1);
  };

  ECCommand2 = digitalRead(gpioEC2);
  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (ECCommand2 != EcStatus2) {
    EcStatus2 = ECCommand2;
    digitalWrite(gpioEC2, EcStatus2);
  };
}
static unsigned long lastEcRead = 0;
void EC_ONE::cond_sol_fsa() 
{
  if (nState==0) { 
    nTmpState=8; lastEcRead = millis(); 
//    Serial.printf("EC_ONE:nCycleEC = %d\n", nCyclesEC);
} 
  if (nState==8) { 
    if ((millis() - lastEcRead) >= nCyclesEC) {
      nTmpState=9; 
    }
  } 
  if (nState==9) { y13(); nTmpState=1;} 
  if (nState==1) { y1(); nTmpState=2; lastEcRead = millis(); } 
  if (nState==2) { 
    if ((millis() - lastEcRead) >= nDelayEC) {
      nTmpState=3; 
    }
  } 
  if (nState==3) { y3(); nTmpState=4; 
    lastEcRead = millis();
  } 
  if (nState==4) { 
    if ((millis() - lastEcRead) >= nDelayEC) {
      nTmpState=5; 
    }
  }
  if (nState==5) { y5(); nTmpState=6; } 
  if (nState==6) { y14(); nTmpState=7; } 
  if (nState==7) { y6(); nTmpState=0; } 
  
  if (nTmpState != nState) { nState = nTmpState; }
}
#endif