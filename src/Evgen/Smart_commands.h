/* Smart_commands.h */
#ifndef SMART_COMMANDS
#define SMART_COMMANDS

/* команды мастеру */
#define HAND_SHAKE_ERR           "NoOkError"

#define MCMD_HAND_SHAKE  		0x2020
#define MCMD_ECHO        		0x01
#define MCMD_IDENTIFY        		0x80 //запрос у сервера
#define MCMD_INTRODUCESELF     		0x81 //передача данных о себе серверу

#define  MCMD_GETINFO       0x02 //запрос информации
#define  MCMD_VIRT_UART	    0x03 //виртуальный уарт
#define  MCMD_GETCONFIG		0x04 //запрос информации о конфигурации
#define  MCMD_DATA_FROM	    	0x05 //данные от станции
#define  MCMD_DATA_TO	    	0x06 //передать данные/команду станции
#define  MCMD_SETCONFIG		0x07 //установить информацию о конфигурации
#define  MCMD_SETDEVUPTIME      0x08 //установить информацию об аптайме и  количестве просыпаний устройства
#define  MCMD_DATA   	    	0x09 //передать данные (отладка)

#define  MCMD_GETTIME		0x10 // читать/задать время RTC
#define  MCMD_SETTIME		0x11 // читать/задать время RTC
#define  MCMD_GETDATA				0x12 //получить данные
#define  MCMD_TESTCMD				0x13 
#define  MCMD_TESTCMDANSWER			0x14 
#define  MCMD_OT_INFO	    0x21 //Open Therm info
#define  MCMD_GET_OT_INFO	0x23 // get Open Therm info
#define  MCMD_SET_OT_DATA	0x24 // set Open Therm data
#define  MCMD_OT_DEBUG  0x30 // set/get Open Therm debug 0/1
#define  MCMD_GET_CAP   0x31 // get capabilities

#define  MCMD_GETLOG		    0x15 //запрос лога
#define  MCMD_LOGON		    	0x16 // лог вкючить-выключить
#define  MCMD_GET_PAR       0x17 //
#define  MCMD_GET_ADC       0x18 //

#define  SCMD_GET_STS		    0x30 // get controller sts  (server ask controller)
#define  SCMD_GET_HAND_SHAKE	0x31 //   server ask handhake
#define  CCMD_SEND_STS_S	0x40 // controller send sts  (controller send server)
#define  ACMD_ASK_STS_S		0x50 // applcation ask is controller known  (applcation ask server)
#define  ACMD_GET_STS_S		0x51 // applcation get controller sts  (applcation ask server)

#define  ACMD_SET_STATE_C	0x52 // applcation set controller state  (applcation send to controller)
#define  ACMD_SET_STATE_S	0x53 // applcation set controller state via server (applcation send to server)
#define  SCMD_SET_STATE_C	0x54 // server set controller state (server send to controller)

#define  CCMD_SEND_OTLOG_S	0x55 // controller send server OT log (controller send to server)
#define  SCMD_SEND_OTLOG_C	0x56 // server send answer to controller OT log (server send to controller)



#endif //SMART_COMMANDS

