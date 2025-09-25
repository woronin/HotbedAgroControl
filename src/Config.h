//#define IF_VCPA
#define IF_SWITCH
#define AUTO_CONNECT
//#define LASER
//#define MQTT_LASER
//#define MQTT_M6
#define MQTT_HOMEASSIST
//#define MQTT_ERR
#define IFMQTT
#define IF_AMT1001
#define IF_DS18D20
#define IF_PHandTEMP
////////////////////////// константы для алгоритма EC
//#define IF_EC                     // исходный алгоритм расчета
//#define IF_EC_ONE                 // расчет EC за один цикл
#define IF_EC_2DELAY                // алгоритм с двумя задержками (используем сейчас)
#define DEBUG_EC                    // вывод отладочной информации
//#define IF_OM                     // расчет в омах
//#define IF_KOTSUBA_EC             // расчет Коцубы
#define IF_EC_FILTER_SENSORS      // значение датчиков пропускаются через фильтр
//#define IF_EC_together_SENSORS      // два измерения без паузы (одно за другим)
///////////////////////////////////////////////////
#define IF_LUX
#define IF_RELAY
#define IF_SENSORLEVEL
//#define MY_DEBUG
#define DEBUG_FSM
//#define MY_DEBUG_LUX
//#define MY_DEBUG_SENSOPRLEVEL
#define  HOMEASSIST_USE 
#define  PAGE_OSCILLOSCOPE 
#define  PAGE_DEBUG 
//#define  PAGE_MQTT 
//#define  DGTFILTERBAT 
#define  IF_FILTER_AMT1001 
// Константы вывода отладочной информации
// Для отмены вывода закомментировать
//#define DEBUG_AMT1001
//#define DEBUG_LUX
//#define DEBUG_DS18D20
//#define DEBUG_PH          // вывод по PH
