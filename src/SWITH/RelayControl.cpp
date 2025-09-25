#include <Arduino.h>
#include <PubSubClient.h>
//extern PubSubClient client;

// Номера выводов для цифровых входов
// Номера выводов для подключения реле
//const int gpioRelay1  = 14;
//const int gpioRelay2  = 15;
//const int gpioRelay3  = 16;
int gpioRelay1  = 25;
int gpioRelay2  = 26;
int gpioRelay3  = 27;
extern bool led1State;
extern bool led2State;
extern bool led3State;
// Логические уровни для включения реле
// в данном случае предполагается, что реле включается высоким уровнем на выходе
const char lvlRelayOn  = 0x1;
const char lvlRelayOff = 0x0;
/**************************************************************************
 * Глобальные переменные
 *************************************************************************/
// Текущее состояние реле
char relayStatus1 = lvlRelayOff;
char relayStatus2 = lvlRelayOff;
char relayStatus3 = lvlRelayOff;
// Топики для внешнего управления реле
const char* mqttTopicControlRelay1   = "demo/relay1/control";
const char* mqttTopicControlRelay2   = "demo/relay2/control";
const char* mqttTopicControlRelay3   = "demo/relay3/control";
const int   mqttRelayControlQos      = 1;
// Топики для публикации состояния реле
const char* mqttTopicStatusRelay1    = "demo/relay1/status";
const char* mqttTopicStatusRelay2    = "demo/relay2/status";
const char* mqttTopicStatusRelay3    = "demo/relay3/status";
// Текстовое отображение для состояния реле
const char* mqttRelayStatusOn1       = "1";
const char* mqttRelayStatusOn2       = "on";
const char* mqttRelayStatusOff1      = "0";
const char* mqttRelayStatusOff2      = "off";
const bool  mqttRelayStatusRetained  = false; 
// Функция обратного вызова при поступлении входящего сообщения от брокера
void RelaymqttOnIncomingMsg(char* topic, byte* payload, unsigned int length)
{
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _payload;
  for (unsigned int i = 0; i < length; i++) {
    _payload += String((char)payload[i]);
  };
  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.print(_payload.c_str());
  Serial.println();

  // Сравниваем с топиками
  String _topic(topic);
  if (_topic.equals(mqttTopicControlRelay1)) {
    // это топик управления реле 1
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayStatus1 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayStatus1 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay2)) {
    // это топик управления реле 2
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayStatus2 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayStatus2 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay3)) {
    // это топик управления реле 3
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayStatus3 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayStatus3 = lvlRelayOff;
    };
  } else {
    Serial.println("Failed to recognize incoming topic!");
  };
}

// Управление реле
extern bool bIf_MQTT;
void relayControl()
{
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (relayStatus1 != led1State) {
    relayStatus1 = led1State;
    digitalWrite(gpioRelay1, relayStatus1);
    if (bIf_MQTT) {
      // Отправляем подтверждение, что команда получена и обработана
//      client.publish(mqttTopicStatusRelay1, (relayStatus1 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    }
    // Можно вывести сообщение в лог
    Serial.print("Relay 1 has changed its state: ");
    Serial.println(relayStatus1 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };
  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (relayStatus2 != led2State) {
    relayStatus2 = led2State;
    digitalWrite(gpioRelay2, relayStatus2);
    if (bIf_MQTT) {
        // Отправляем подтверждение, что команда получена и обработана
//      client.publish(mqttTopicStatusRelay2, (relayStatus2 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    }
    // Можно вывести сообщение в лог
    Serial.print("Relay 2 has changed its state: ");
    Serial.println(relayStatus2 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };
  // Новое состояние реле 3 отличается от текущего, требуется переключение
  if (relayStatus3 != led3State) {
    relayStatus3 = led3State;
    digitalWrite(gpioRelay3, relayStatus3);
    if (bIf_MQTT) {
        // Отправляем подтверждение, что команда получена и обработана
//      client.publish(mqttTopicStatusRelay3, (relayStatus3 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    }
    // Можно вывести сообщение в лог
//    Serial.print("Relay 3 has changed its state: ");
//    Serial.println(relayStatus3 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };
}
