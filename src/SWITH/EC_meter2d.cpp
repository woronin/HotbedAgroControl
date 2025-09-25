#include "Config.h"
#if defined(IF_EC_2DELAY)
#include <Arduino.h>
//#include <PubSubClient.h>
#include "Config.h"
#include "EC_meter2d.h"

EC_meter::EC_meter(String strN):LFsaAppl(strN) 
{ 
    pinMode(d1, INPUT); pinMode(d2, INPUT);
}

void EC_meter::run() { 
    cond_sol_fsa();
}
#if not defined(IF_KOTSUBA_EC)
int EC_meter::x1() { return var < nCyclesEC; }

void EC_meter::y1() {
  digitalWrite(d2, HIGH);
  digitalWrite(d1, LOW);
} 

void EC_meter::y3() {
/*
  double dDAT;
  double dD = analogRead(a1);
  double pin_mVolt = esp_adc_cal_raw_to_voltage(dD, adc_chars);  
  dDAT = pin_mVolt;  
//  dDAT = dD;  
*/  
  dDap = analogRead(a1);
  #if defined(IF_EC_FILTER_SENSORS)
    dArrayAp.Put(dDap, nN, true);
    dDATap = dArrayAp.Get(nN, true);
  #else
    dDATap = dDap;
  #endif    

//  Serial.print("dD(ap)=");Serial.print(dD);
//  dDAT = dD;  
  ap = 0 + dDATap + ap;

  digitalWrite(d1, HIGH);
  digitalWrite(d2, LOW);
}
void EC_meter::y4() {} 
void EC_meter::y5() {
/*
  double dDAT;
  double dD = analogRead(a1);
  double pin_mVolt = esp_adc_cal_raw_to_voltage(dD, adc_chars);  
  dDAT = pin_mVolt;  
//  dDAT = dD;  
  an = (0 + dDAT) + an;
*/
  dDan = analogRead(a1);
  #if defined(IF_EC_FILTER_SENSORS)
    dArrayAn.Put(dDan, nN, true);
    dDATan = dArrayAn.Get(nN, true);
  #else
    dDATan = dDan;
  #endif
//  Serial.print("dD(an)=");Serial.println(dD);
//  dDAT = dD;  
  an = (0 + dDATan) + an;
  digitalWrite(d1, LOW);
  var++;
}
void EC_meter::y6() {
/*  
    digitalWrite(d1, LOW); digitalWrite(d2, LOW);
    // Усреднение АЦП при положительной ap и отрицательной an фазе
    // Усреднение АЦП при положительной фазе
    float ap_raw=(float)ap/var;
// Усреднение АЦП при отрицательной фазе
    float an_raw=(float)an/var;
// Расчет поляризации раствора 
    float po;
    po = ap_raw-an_raw;
#if defined(DEBUG_EC)    
    Serial.printf("\nap_raw=%f an_raw=%f po=%f\n", ap_raw, an_raw, po);
#endif    
//    EC_acc = 8.3451 * exp(-0.0028* ap_raw);
     EC_acc = fBasys * pow(po, fDegree); // Замер 2 - 4А
*/
    digitalWrite(d1, LOW); digitalWrite(d2, LOW);
    // Усреднение АЦП при положительной ap и отрицательной an фазе
    // Усреднение АЦП при положительной фазе
    float ap_raw=(float)ap/var;
// Усреднение АЦП при отрицательной фазе
    float an_raw=(float)an/var;
// Расчет поляризации раствора 
    float po;
	if (an_raw==0) an_raw = ap_raw;
    po = (ap_raw/an_raw)*10;
#if defined(DEBUG_EC)    
    Serial.printf("\ndDap=%.0f dDATap=%.0f dDan=%.0f dDATan=%.0f ", dDap, dDATap, dDan, dDATan);
    Serial.printf("\nap_raw=%.2f an_raw=%.2f po=%.2f var = %d", ap_raw, an_raw, po, var);
#endif    
//    EC_acc = ((((float)ap + (float)an) / var) / 2);
//    EC_acc = 1287.79 * pow(po, -2.79); // Замер 1 - 4А
  if (po >=4.5 && po<9.5)
    EC_acc = -0.176 * po + 1.67;  // EC 0.1 - 0.7
  else if ( po >=2.8 && po < 4.5)
    EC_acc = -0.81 * po + 5.21;  // EC 0.7 - 2.5
  else if ( po >2.0 && po < 2.8)
    EC_acc = -2.63 * po + 11.23; // EC 2.5- 4.5
  else if (po >= 9.5)
    EC_acc = 0.0;
  else if ( po <= 2.0 )
    EC_acc = 5.55;
//    EC_acc = fBasys * pow(po, fDegree); // Замер 2 - 4А

  #if defined (DEBUG_EC)
    Serial.print(" EC: ");
    Serial.print(EC_acc);
  #endif
}   

void EC_meter::y13() {
  pinMode(d1, OUTPUT); pinMode(d2, OUTPUT);
  var = 0;
  ap = 0, an = 0;
}   

void EC_meter::y14() { pinMode(d1, OUTPUT); pinMode(d2, OUTPUT); }   
static unsigned long lastEcRead = 0;
static unsigned int nTimeEC = 2000;
#if not defined(IF_EC_together_SENSORS)
void EC_meter::cond_sol_fsa() 
{
  if (nState==0) { 
    nTmpState=8; lastEcRead = millis(); 
  } 
  if (nState==8) { if ((millis() - lastEcRead) >= nTimeEC) { y13(); nTmpState=1; }} 
  if (nState==1 && x1()) { y1(); nTmpState=2; lastEcRead = millis(); } 
  if (nState==1 && !x1()) { y6(); nTmpState=7; lastEcRead = millis();} 
  if (nState==2) { 
    if ((millis() - lastEcRead) >= nDelayEC) { nTmpState=3; }
  } 
  if (nState==3) { 
    y3(); 
    nTmpState=4; lastEcRead = millis(); 
  } 
  if (nState==4) { 
    if ((millis() - lastEcRead) >= nDelayEC) { nTmpState=5; }
  }
  if (nState==5) { 
    y5(); 
    nTmpState=6; 
  } 
  if (nState==6) { y14(); nTmpState=1;  } 
  if (nState==7) { 
    if ((millis() - lastEcRead) >= 2*nDelayEC) { nTmpState=0; }
  } 
  
  if (nTmpState != nState) { nState = nTmpState; }
}
#endif

#if defined(IF_EC_together_SENSORS)
void EC_meter::cond_sol_fsa() 
{
  if (nState==0) { 
    nTmpState=8; lastEcRead = millis(); 
  } 
  if (nState==8) { if ((millis() - lastEcRead) >= nTimeEC) { y13(); nTmpState=1; }} 
  if (nState==1 && x1()) { y1(); nTmpState=3; } 
  if (nState==1 && !x1()) { y6(); nTmpState=0; } 
  if (nState==3) { 
    y3(); 
    y5();
    y14();
    nTmpState=1; 
  } 
//  if (nState==4) { nTmpState=6; }
//  if (nState==6) { y14(); nTmpState=1;  } 
//  if (nState==7) { 
//    if ((millis() - lastEcRead) >= 2*nDelayEC) { nTmpState=0; }
//  } 
  if (nTmpState != nState) { nState = nTmpState; }
}
#endif

#endif
void EC_meter::ECMeterControl()
{
  ECCommand1 = digitalRead(gpioEC1);
  if (ECCommand1 != EcStatus1) {
    EcStatus1 = ECCommand1;
    digitalWrite(gpioEC1, EcStatus1);
  };

  ECCommand2 = digitalRead(gpioEC2);
  if (ECCommand2 != EcStatus2) {
    EcStatus2 = ECCommand2;
    digitalWrite(gpioEC2, EcStatus2);
  };
}
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1; // (ADC_UNIT_1 or ADC_UNIT_2)
/* Задаем пины Ap, An, канал АЦП */
void EC_meter::setup_Ec_meter(uint8_t _Ap_Pin, uint8_t _An_Pin, uint8_t _Adc1_Chan)
{
    Ap_Pin = _Ap_Pin;
    An_Pin = _An_Pin;
    
    pinMode(Ap_Pin, OUTPUT);
    pinMode(An_Pin, OUTPUT);
    digitalWrite(Ap_Pin, LOW);
    digitalWrite(An_Pin, LOW);
    switch(_Adc1_Chan)
    {   case 0: Adc1_Chan = ADC1_CHANNEL_0; break;
        case 3: Adc1_Chan = ADC1_CHANNEL_3; break;
        case 4: Adc1_Chan = ADC1_CHANNEL_4; break;
        case 5: Adc1_Chan = ADC1_CHANNEL_5; break;
        case 6: Adc1_Chan = ADC1_CHANNEL_6; break;
        case 7: Adc1_Chan = ADC1_CHANNEL_7; break;
    }

    pinMode(Adc1_Chan,ANALOG);
    adc1_calibrate(0);
    adc1_config(Adc1_Chan); 
}

void EC_meter::adc1_calibrate(int mode) 
{ adc1_config(ADC1_CHANNEL_0);
  if(mode)
      check_efuse();
//Characterize ADC at particular atten
  if(adc_chars == NULL) {
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  }
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
  if(mode)
  {
    Serial.println("ADC number:\t" + String(adc_chars->adc_num));
    Serial.println("ADC attenuation:\t" + NumToAtten(adc_chars->atten));
    Serial.println("ADC bit width:\t" + NumToWidth(adc_chars->bit_width));
    Serial.println("ADC coeff_a:\t" + String(adc_chars->coeff_a));
    Serial.println("ADC coeff_b:\t" + String(adc_chars->coeff_b));
    Serial.println("ADC VRef:\t" + String(adc_chars->vref));
    //Check type of calibration value used to characterize ADC
    print_char_val_type(val_type);  
  }
}

void EC_meter::print_char_val_type(esp_adc_cal_value_t val_type) {
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    Serial.println("Characterized using Two Point Value\n");
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.println("Characterized using eFuse Vref");
  } else {
    Serial.println("Characterized using Default Vref");
  }
}

void EC_meter::adc1_config(int adc_chan) 
{ adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten((adc1_channel_t) adc_chan, ADC_ATTEN_DB_12);
}

void EC_meter::check_efuse() 
{
//Check TP is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
    Serial.println("eFuse Two Point: Supported");
  } else {
    Serial.println("eFuse Two Point: NOT supported");
  }
  //Check Vref is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
    Serial.println("eFuse Vref: Supported");
  } else {
    Serial.println("eFuse Vref: NOT supported");
  }
}

String EC_meter::NumToAtten(int atten) 
{
  switch (atten)
  {
    case 0:
      return "ADC_ATTEN_DB_0. No chages for the input voltage";
    case 1:
      return "ADC_ATTEN_DB_2_5. The input voltage will be reduce to about 1/1.34.";
    case 2:
      return "ADC_ATTEN_DB_6. The input voltage will be reduced to about 1/2";
    case 3:
      return "ADC_ATTEN_DB_11. The input voltage will be reduced to about 1/3.6";
  }
  return "Unknown attenuation.";
}

String EC_meter::NumToWidth(int width) 
{
  switch (width)
  {
    case 0:
      return "ADC_WIDTH_BIT_9. ADC capture width is 9Bit";
    case 1:
      return "ADC_WIDTH_BIT_10. ADC capture width is 10Bit";
    case 2:
      return "ADC_WIDTH_BIT_11. ADC capture width is 11Bit";
    case 3:
      return "ADC_WIDTH_BIT_12. ADC capture width is 12Bit";
  }
  return "Unknown width.";
}

#if defined(IF_KOTSUBA_EC)    
void EC_meter::cond_sol_fsa() 
{
    switch(sts)
    { case 0:

        double dDap;
        double dDan;
        rc = Ec_meter_raw(16, 10, dDap, dDan); //16 измерений с задержкой 10 мкс
        sts = 1;
        #if defined(DEBUG_EC)    
            Serial.printf("\ndDap=%.0f dDan=%.0f ", dDap, dDan);
        #endif    

      break;
      case 1:
        rc = get_Ec(EcK, R_Divider, R, Ec); //получить результаты измерения
        EC_acc = Ec;
        Serial.printf("R=%g Ec=%g\n",R, Ec);
        sts = 0;
      break;
    }
}

//nd измерений напряжений с задержкой delay мкс между измерениями
//nd < NumRead
int EC_meter::Ec_meter_raw(int nd, int delay, double &dDap, double &dDan)
{   int i, va1,va2;

    if(nd >= NumRead)
            return 1;

    for(i=0;i<nd; i++)
    {
        digitalWrite(Ap_Pin, HIGH);
        va1 = get_adc1(Adc1_Chan);
        
        digitalWrite(Ap_Pin, LOW);
        digitalWrite(An_Pin, HIGH);

        va2 = get_adc1(Adc1_Chan);
        
        digitalWrite(An_Pin, LOW);

    //  t = micros();
    //  buf0[i].t = t;
        buf0[i].a = va1;
        buf0[i].b = va2;
        
        if(delay > 0)
         delayMicroseconds(delay);
    }
    dDap = va1; dDan = va2;

    nread = nd;
    return 0;
}

/*
K коэффициент порядка (L/S) для электрода
R0 -  сопротивление делителя
Получаем из raw данных АЦП (nread элементов buf0)
напряжения в мВ по калибровке АЦП? по которым получаем среднее сопротивление
По среднему сопротивлению находим Ec = K / R;
*/
int EC_meter::get_Ec(float K, float R0, float &R, float &Ec)
{   
  int i, rc;
  int ir1=0, ir2=0;
  float r, r1, rn;
  if(nread <= 0)
      return 1;
  r = 0.;
  for(i=0;i<nread;i++)
  {   
    ir1 = Get_adc1_mv(buf0[i].a);
    ir2 = Get_adc1_mv(buf0[i].b);
    if(ir2 > 0)
    {   
      r += float(ir1)/float(ir2);
    } 
    else {
      r += float(ir1);
    }        
  }
  r /= float(nread);
  R = r * R0;
  Ec = -1.;
  if(R > 0.)
  {   Ec = K / R;
  } else {
      return 0;
  }
  return 0;
}

int EC_meter::get_adc1(int adc_chan)
{  return adc1_get_raw((adc1_channel_t)adc_chan);
}

int EC_meter::Get_adc1_mv(int reading)
{
//  adc_chars = 1073449496;
//  Serial.printf("reading=%d adc_chars = %d\n", reading, adc_chars);
 // основная функция, преобразующая из измерянных сырых самплов (0..4095) в милливольты 
 // исходя из коэффициентов, измеренных при автокалибровке и сохраненных в струтуре  adc_chars
  uint32_t pin_mVolt=0.01;
  pin_mVolt = esp_adc_cal_raw_to_voltage(reading, adc_chars);  
 return pin_mVolt;
}

#endif
#endif
