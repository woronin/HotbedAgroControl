#include "Config.h"
#if defined(IF_EC)
#include <Arduino.h>
#include <PubSubClient.h>
#include "Config.h"
#include "EC_meter.h"

#define Voron
#define EC_VALUE 1000
//#define IF_FILTERS
EC_meter::EC_meter(String strN):LFsaAppl(strN) 
{ 
    pinMode(d1, INPUT); pinMode(d2, INPUT);
}

void EC_meter::run() { 
//  nState = 0;
//Serial.println("cond_sol_fsa()");
cond_sol_fsa();
//  ECMeterControl(); 
}

int EC_meter::x1() { return var < nCyclesEC; }

void EC_meter::y1() {
  digitalWrite(d2, HIGH);
  digitalWrite(d1, LOW);
} 
void EC_meter::y2() { 
  delay(nDelayEC); 
/*  
  Serial.print("y2():");
  Serial.print("var=");
  Serial.print(var);
  Serial.print("; nDelayEC=");
  Serial.println(nDelayEC);
*/  
}
void EC_meter::y3() {
  double dDAT;
  double dD = analogRead(a1);
  Serial.print("p=");Serial.print(dD);
#if defined (IF_FILTERS)  
  dArrayAp.Put(dD, nN, true);
  dDAT = dArrayAp.Get(nN, true);
#else
  dDAT = dD;  
#endif  
  ap = 0 + dDAT + ap;
//  Serial.printf(".EC(dat)= %d\n", dat);
//  ap = 0 + dat + ap;
//  Serial.printf(".EC= %d\n", ap);
  digitalWrite(d2, LOW);

  digitalWrite(d1, HIGH);
  digitalWrite(d2, LOW);

#if defined (DEBUG_EC)
  Serial.print("-");
#endif
}
void EC_meter::y4() {} 
void EC_meter::y5() {
  double dDAT;
  double dD = analogRead(a1);
  Serial.print("n=");Serial.print(dD);
#if defined (IF_FILTERS)  
  dArrayAp.Put(dD, nN, true);
  dDAT = dArrayAp.Get(nN, true);
#else
  dDAT = dD;  
#endif  
//  an = (4095 - dDAT) + an;
  an = (0 + dDAT) + an;
//  Serial.printf(".EC(dat_an)= %d\n", dat);
//  an = 1023 - dat + an;
//  an = 0 + dat + an;
  digitalWrite(d1, LOW);
  var++;
}
void EC_meter::y6() {
#if not defined (Voron)  
  #if defined (MY_DEBUG)
    Serial.println("y6");
  #endif
  pinMode(d1, INPUT); pinMode(d2, INPUT);
  // Усреднение АЦП при положительной ap и отрицательной an фазе
// Усреднение АЦП при положительной фазе
  float ap_raw=(float)ap/(var-1);
// Усреднение АЦП при отрицательной фазе
  float an_raw=(float)an/var;
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
  EC_acc = ((((float)ap + (float)an) / var) / 2);
#endif  
//  EC_acc = EC_acc * (3.3 / 4095.0) - 0.96;  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)

  // Вывод считанных данных в лог
  #if defined (DEBUG_EC)
    Serial.print("EC Value: ");
    Serial.println(EC);
  #endif
//  int n = random(20);
//  FSetSleep(n);
#else
  #if defined (MY_DEBUG)
    Serial.println("y6");
  #endif
  pinMode(d1, INPUT); pinMode(d2, INPUT);
  // Усреднение АЦП при положительной ap и отрицательной an фазе
// Усреднение АЦП при положительной фазе
  float ap_raw=(float)ap/(var);
// Усреднение АЦП при отрицательной фазе
  float an_raw=(float)an/(var);
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
   Serial.printf("\nap_raw=%.0f an_raw=%.0f po=%.2f\n", ap_raw, an_raw, po);
   Serial.printf("ap=%d an=%d po=%d\n", ap, an, po);
//  
#if defined(IF_OM)
  EC_acc = ((float)ap/(float)an)*10;
#else 
  EC_acc = ((((float)ap + (float)an) / var) / 2);
#endif  
  // Вывод считанных данных в лог
  #if defined (DEBUG_EC)
    Serial.print("EC Value: ");
    Serial.println(EC);
  #endif
#endif
}   

void EC_meter::y13() {
//  int sensorValue = analogRead(sensEC);  // Чтение аналогового значения
//  float voltage = sensorValue * (3.3 / 4095.0);  // Преобразование в напряжение (ESP32 имеет 12-битный АЦП)

  pinMode(d1, OUTPUT); pinMode(d2, OUTPUT);
  var = 0;
  ap = 0, an = 0;
//  Serial.print("<WorkEC");
}   

void EC_meter::y14() { pinMode(d1, OUTPUT); pinMode(d2, OUTPUT); }   

// Контроль состояния входов
void EC_meter::readDigitalInputsEC()
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
void EC_meter::readECValue() {

}
// Управление ES_meter
void EC_meter::ECMeterControl()
{
  ECCommand1 = digitalRead(gpioEC1);
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (ECCommand1 != EcStatus1) {
    EcStatus1 = ECCommand1;
    digitalWrite(gpioEC1, EcStatus1);
    // Можно вывести сообщение в лог
//    Serial.println("EC1 has changed its state: ");
  };

  ECCommand2 = digitalRead(gpioEC2);
  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (ECCommand2 != EcStatus2) {
    EcStatus2 = ECCommand2;
    digitalWrite(gpioEC2, EcStatus2);
    // Можно вывести сообщение в лог
//    Serial.println("ES2 has changed its state: ");
  };
}
//Функция замера электропроводности в RAW
float EC_meter::fConductivity(int d1, int d2, int a1, long cnt)
{
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT); 
  long var=1;
  unsigned long ap=0;
  unsigned long an=0;  
  while(var < cnt){

    digitalWrite(d2, HIGH);
    ap=0+analogRead(a1)+ap;
    digitalWrite(d2, LOW);
 
    digitalWrite(d1, HIGH);
    
    an=1023-analogRead(a1)+an;
    digitalWrite(d1, LOW); 
    var++;
  }
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
// Усреднение АЦП при положительной фазе
  float ap_raw=(float)ap/var;
// Усреднение АЦП при отрицательной фазе
  float an_raw=(float)an/var;
// Расчет поляризации раствора 
  float po = ap_raw-an_raw;
// Исключение влияние поляризаци
  return (((float)ap+(float)an)/var/2);

}

static unsigned long lastEcRead = 0;
static unsigned int nTimeEC = 100;
void EC_meter::cond_sol_fsa() 
{
  if (nState==0) { 
    nTmpState=8; lastEcRead = millis(); 
  //  Serial.printf("EC_ONE:nTimeEC = %d\n", nTimeEC);
  } 
  if (nState==8) { if ((millis() - lastEcRead) >= nTimeEC) { nTmpState=9; }} 
  if (nState==9) { y13(); nTmpState=1;} 
  if (nState==1 && x1()) { y1(); nTmpState=2; lastEcRead = millis(); } 
  if (nState==1 && !x1()) { y6(); nTmpState=0; } 
  if (nState==2) { 
    if ((millis() - lastEcRead) >= nDelayEC) { nTmpState=3; }
  } 
  if (nState==3) { y3(); nTmpState=4; lastEcRead = millis(); } 
  if (nState==4) { 
    if ((millis() - lastEcRead) >= nDelayEC) { nTmpState=5; }
  }
  if (nState==5) { y5(); nTmpState=6; } 
  if (nState==6) { y14(); nTmpState=1; } 
  
  if (nTmpState != nState) { nState = nTmpState; }
}

float EC_meter::cond_sol(int d1, int d2, int a0) 
{
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  long var = 1;
  unsigned long ap = 0;
  unsigned long an = 0;
 
  while (var < nCyclesEC) {
 
    digitalWrite(d2, HIGH);
    digitalWrite(d1, LOW);
    delay(nDelayEC);
    double dA0P = analogRead(a0);
    dArrayAp.Put(dA0P, nN, true);
    dA0P = dArrayAp.Get(nN, true);
    ap = 0 + dA0P + ap;

    digitalWrite(d2, LOW);
 
    digitalWrite(d1, HIGH);
    digitalWrite(d2, LOW);
    Serial.print("-");
    delay(nDelayEC);
    double dA0N = analogRead(a0);
    dArrayAn.Put(dA0N, nN, true);
    dA0N = dArrayAn.Get(nN, true);
    an = 1023 - dA0N + an;

    digitalWrite(d1, LOW);
    var++;
  }
  Serial.println("-");
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  // Усреднение АЦП при положительной ap и отрицательной an фазе
  return (((float)ap + (float)an) / var / 2);
}
#endif