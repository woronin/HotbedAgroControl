#include "Config.h"
#if defined(IF_EC_2DELAY)
#include <Arduino.h>
#include "Config.h"
#include "EC_meter2d.h"
static unsigned long lastEcRead = 0;
static unsigned int nTimeEC = 10000;
#if defined (IF_PHandTEMP)
  #include "SWITCH/PhAndTemperature.h"
  extern PhAndTemperature PHAndTemperature;

#endif
EC_meter::EC_meter(String strN):LFsaAppl(strN) 
{ 
    pinMode(d1, INPUT); pinMode(d2, INPUT);
}

void EC_meter::run() { 
  if (nState==0 && x5()) { nTmpState=8; y2(); } 
  if (nState==8 && x2()) { y13(); nTmpState=1; } 
  if (nState==1 && x1()) { y1(); nTmpState=3; } 
  if (nState==1 && !x1()) { y6(); nTmpState=0; } 
  if (nState==3) { y3(); y5(); y14(); nTmpState=1; } 
  if (nTmpState != nState) { nState = nTmpState; }
}
int EC_meter::x1() { return var < nCyclesEC; }
int EC_meter::x2() { return (millis() - lastEcRead) >= nTimeEC; }
int EC_meter::x5() { 
#if defined (IF_PHandTEMP)
  return PHAndTemperature.FGetState() == "0"; 
#endif  
  return true; 
}

void EC_meter::y1() { digitalWrite(d2, HIGH); digitalWrite(d1, LOW); } 
void EC_meter::y2() { lastEcRead = millis(); }   
void EC_meter::y3() {
  dDap = analogRead(a1);
  #if defined(IF_EC_FILTER_SENSORS)
    dArrayAp.Put(dDap, nN, true);
    dDATap = dArrayAp.Get(nN, true);
  #else
    dDATap = dDap;
  #endif    
  ap = 0 + dDATap + ap;
  digitalWrite(d1, HIGH);
  digitalWrite(d2, LOW);
}
void EC_meter::y5() {
  dDan = analogRead(a1);
  #if defined(IF_EC_FILTER_SENSORS)
    dArrayAn.Put(dDan, nN, true);
    dDATan = dArrayAn.Get(nN, true);
  #else
    dDATan = dDan;
  #endif
  an = (0 + dDATan) + an;
  digitalWrite(d1, LOW);
  var++;
}
void EC_meter::y6() {
    digitalWrite(d1, LOW); digitalWrite(d2, LOW);
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
  if (po>4.31 && po<17) EC_acc = -0.05 * po + 0.87;
  else if ( po >2.8 && po < 4.3) EC_acc = -1.23 * po + 6.04;
  else if (po >= 17) EC_acc = 5.0;
  else if ( po <= 2.8 ) EC_acc = 0.0;

  #if defined (DEBUG_EC)
    Serial.print(" EC: ");
    Serial.print(EC_acc);
  #endif
}   
void EC_meter::y13() {
  pinMode(d1, OUTPUT); pinMode(d2, OUTPUT);
  var = 0; ap = 0, an = 0;
}   
void EC_meter::y14() { pinMode(d1, OUTPUT); pinMode(d2, OUTPUT); }   

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
  if(mode) check_efuse();
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
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
    Serial.println("eFuse Two Point: Supported");
  } else {
    Serial.println("eFuse Two Point: NOT supported");
  }
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

#endif
