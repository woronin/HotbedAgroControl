/* SmartDevice.hpp */
#ifndef SMARTDEVICE
#define SMARTDEVICE

#include "Smart_Config.h"



#define MASTER_BIOSCODE   23
#define MASTER_VERSION     1
#define MASTER_SUBVERSION  0
#define MASTER_SUBVERSION1 0
/***************/

#define U8   unsigned char
#define U16  unsigned short
#define U32  unsigned int
#define S8   char
#define S16  short
#define S32  int
#define Bool unsigned char;
#define TRUE  1
#define FALSE 0
#define PACKED  /* */

#define FLASH_MARK 0x1AA11BBf
#define FLASH_WRITESIZE (36+4*16)

//макрос для автоматического определения размера статической части класса для записи-чтения во флеш
#define FLASH_WRITESIZE0 ((int)(&this->sts) - (int)(&this->mark))

class SmartDevice
{
 public:
  const  int mark;  /* пометка для определения наличия во флеше  */
  const  int size;  /* размер для чтения-записи-сравнения версий */
/* константы */
  const int BiosCode;     /* код биоса */
  const int Vers;         /* версия */
  const int SubVers;      /* подверсия */
  const int SubVers1;     /* версия подверсии */
#if defined(ARDUINO_ARCH_ESP8266)
  static char  BiosDate[12];     /* дата компиляции биоса */
#elif defined(ARDUINO_ARCH_ESP32)
  const char  BiosDate[12];     /* дата компиляции биоса */
#endif
                         
static char LocalUrl[24]; /* smth like http://192.168.200.201 */
  int IdNumber;            /* номер устройства */
#if defined(ARDUINO_ARCH_ESP8266)
  unsigned char Mac[6];   /* mac - адрес */
  unsigned char  _foo[2];  
#elif defined(ARDUINO_ARCH_ESP32)
  unsigned char Mac[6];   /* mac - адрес */
#endif
  int status;
  int sts;                 /* состояние       */  
  int sts_next;            /* состояние на следующий такт  */
  
#if defined(ARDUINO_ARCH_ESP8266)
  SmartDevice(void):mark(FLASH_MARK), size (FLASH_WRITESIZE0), 
               BiosCode(MASTER_BIOSCODE),Vers(MASTER_VERSION), 
               SubVers(MASTER_SUBVERSION), SubVers1(MASTER_SUBVERSION1),
							 ReservParam()
#elif defined(ARDUINO_ARCH_ESP32)
  SmartDevice(void):mark(FLASH_MARK), size (FLASH_WRITESIZE0), 
               BiosCode(MASTER_BIOSCODE),Vers(MASTER_VERSION), 
               SubVers(MASTER_SUBVERSION), 
               SubVers1(MASTER_SUBVERSION1), 
               BiosDate(__DATE__)
#endif
			   
  {  // int i;  
	sts = sts_next = 0;
  status = -1;
    Mac[0]= Mac[1] = Mac[2] =Mac[3] =Mac[4] =Mac[5] = 0; 
    IdNumber = 0;

  }
 
};

 #endif //SMARTDEVICE
