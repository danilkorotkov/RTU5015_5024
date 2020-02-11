/*
 * MC35i.h
 *
 *  Created on: 25 дек. 2019 г.
 *      Author: proizvodstvoograzdenij
 */

#ifndef MC35I_H_
#define MC35I_H_

//#include "/Users/proizvodstvoograzdenij/Library/Arduino15/packages/arduino/hardware/avr/1.8.2/libraries/SoftwareSerial/src/SoftwareSerial.h"
//#include "SoftwareSerial.h"

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define GSM_UART_SPEED		9600																				//	Скорость шины UART на которой требуется работать			(9600 бит/сек)
#define	GSM_TXT_CP866		  0																						//	Название кодировки в которой написан текст.					(паремр функций TXTsendCoding() и TXTreadCoding() указывающий кодировку CP866)
#define	GSM_TXT_UTF8		  1																						//	Название кодировки в которой написан текст.					(паремр функций TXTsendCoding() и TXTreadCoding() указывающий кодировку UTF8)
#define	GSM_TXT_WIN1251		2																						//	Название кодировки в которой написан текст.					(паремр функций TXTsendCoding() и TXTreadCoding() указывающий кодировку WIN1251)
#define	GSM_SMS_CLASS_0		0																						//	Класс отправляемых SMS сообщений.							(паремр функции SMSsendClass() указывающий что отправляются SMS класса 0)
#define	GSM_SMS_CLASS_1		1																						//	Класс отправляемых SMS сообщений.							(паремр функции SMSsendClass() указывающий что отправляются SMS класса 1)
#define	GSM_SMS_CLASS_2		2																						//	Класс отправляемых SMS сообщений.							(паремр функции SMSsendClass() указывающий что отправляются SMS класса 2)
#define	GSM_SMS_CLASS_3		3																						//	Класс отправляемых SMS сообщений.							(паремр функции SMSsendClass() указывающий что отправляются SMS класса 3)
#define	GSM_SMS_CLASS_NO	4																						//	Класс отправляемых SMS сообщений.							(паремр функции SMSsendClass() указывающий что отправляются SMS без класса)
#define GSM_OK				    0																						//	Статус GSM - без ошибок.									(возвращается функцией CALLstatus / status если ошибки не обнаружены и модуль готов к работе)
#define GSM_SPEED_ERR		  1																						//	Не удалось согласовать скорость UART.						(возвращается функцией     status если в функции _begin был сброшен флаг flgSpeed)
#define GSM_UNAVAILABLE		2																						//	Статус GSM - модуль недоступен (AT-команды не выполняются).	(возвращается функцией     status если на команду "AT+CPAS"  пришел ответ "+CPAS:1"       )
#define GSM_UNKNOWN			  3																						//	Статус GSM - неизвестен (AT-команды могут не выполнятся).	(возвращается функцией     status если на команду "AT+CPAS"  пришел ответ "+CPAS:2"       )
#define GSM_SLEEP			    4																						//	Статус GSM - модуль режиме ограниченной функциональности.	(возвращается функцией     status если на команду "AT+CPAS"  пришел ответ "+CPAS:5"       )
#define GSM_SIM_PIN			  5																						//	Требуется ввод PIN1 (CHV1).									(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CPIN:SIM PIN" )
#define GSM_SIM_PUK			  6																						//	Требуется ввод PUK1 и новый PIN1.							(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CPIN:SIM PUK" )
#define GSM_SIM_PIN2		  7																						//	Требуется ввод PIN2 (CHV2).									(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CPIN:SIM PIN2")
#define GSM_SIM_PUK2		  8																						//	Требуется ввод PUK2 и новый PIN2.							(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CPIN:SIM PUK2")
#define GSM_SIM_NO			  9																						//	Нет SIM-карты.												(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CME ERROR:10" )
#define GSM_SIM_FAULT		  10																					//	SIM-карта неисправна.										(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ "+CME ERROR:13" )
#define GSM_SIM_ERR			  11																					//	Неопределённое состояние SIM-карты.							(возвращается функцией     status если на команду "AT+CPIN?" пришел ответ не "+CPIN:READY")
#define GSM_REG_NO			  12																					//	Модем не зарегистрирован в сети оператора.					(возвращается функцией     status если на команду "AT+CREG?" пришел ответ где второй параметр равен 0, 2 или равен значению отсутствующему в документации.
#define GSM_REG_FAULT		  13																					//	Регистрация модема в сети оператора отклонена.				(возвращается функцией     status если на команду "AT+CREG?" пришел ответ где второй параметр равен 3
#define GSM_REG_ERR			  14																					//	Статус регистрации модема в сети оператора не читается.		(возвращается функцией     status если на команду "AT+CREG?" пришел ответ без символов "+CREG".
#define GSM_CALL_ACTIVE		1																						//	Состояние вызова - активное голосовое соединение.			(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть актывный)
#define GSM_CALL_HELD		  2																						//	Состояние вызова - удерживаемый.							(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть удерживаемый)
#define GSM_CALL_END		  3																						//	Состояние вызова - разъединение.							(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть разъединяенмый)
#define GSM_CALL_OUT_DIAL	4																						//	Состояние вызова - исходящий в режиме набора адреса.		(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть исходящий набираемый)
#define GSM_CALL_OUT_BEEP	5																						//	Состояние вызова - исходящий в режиме дозвона.				(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть исходящий дозванивающийся)
#define GSM_CALL_IN_BEEP	6																						//	Состояние вызова - входящий  в режиме дозвона.				(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть входящий дозванивающийся)
#define GSM_CALL_IN_WAIT	7																						//	Состояние вызова - входящий  в режиме ожидания.				(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть входящий ожидающий)
#define GSM_CALL_ERR		  8																						//	Состояние вызова - неопределено.							(возвращается функцией CALLstatus если на команду "AT+CLCC"  пришел список текущих вызовов среди которых есть не описанный в докуметации)
#define GSM_HEADSET			  0																						//	Звук принимается/передаётся с гарнитуры						(паремр функций SOUNDdevice() указывающий использовать наушники и микрофон гарнитуры)
#define GSM_SPEAKER			  1																						//	Звук принимается/передаётся по громкой связи				(паремр функций SOUNDdevice() указывающий использовать динамик и микрофон подключённые к штыревым выводам на плате)
#define GSM_MICROPHONE		2

#define RXBufferSize 120//167
#define TOOBIG 63

class MC35i {

public:
				MC35i(uint8_t = 6, uint8_t = 7);

	bool 		begin(HardwareSerial &i){flgType=0; Port = &i; return _begin();}
	//bool 		begin(SoftwareSerial &i){flgType=1; Port = &i; return _begin();}
	bool 		SendATCommand(String, bool = true, long = 10000);
	uint8_t	SMStotal(void);
	uint8_t	Status(void);
	bool 		CallAvailable(char*);
	uint8_t	SMSsum = 0;
	uint8_t SMStot = 0;
	bool 		SmsAvailable(void);
	void		CallEnd(void);

	bool		SMSread(char*);																                            //	Объявляем  функцию для чтения самой ранней одной SMS		(аргумент функции: указатель на строку для получения текста)
	bool		SMSread(char*, char*);															                      //	Объявляем  функцию для чтения самой ранней одной SMS		(аргумент функции: указатель на строку для получения текста, указатель на строку для получения номера)
	bool		SMSread(char*, char*, char*);													                    //	Объявляем  функцию для чтения самой ранней одной SMS		(аргумент функции: указатель на строку для получения текста, указатель на строку для получения номера, указатель на строку для получения даты)
	bool		SMSread(char*, char*, char*, uint16_t&, uint8_t&, uint8_t&);					    //	Объявляем  функцию для чтения самой ранней одной SMS		(аргумент функции: указатель на строку для получения текста, указатель на строку для получения номера, указатель на строку для получения даты, ссылка для получения идентификатора SMS, ссылка для получения количества SMS,  ссылка для получения номера SMS)

	bool		SMSsend(char*,  char*, uint16_t=0, uint8_t=1, uint8_t=1);			            //	Объявляем  функцию для чтения самой ранней одной SMS		(аргумент функции: текст,                                    номер,                                                                            идентификатор составного SMS сообщения,  количество SMS в составном сообщении, номер SMS в составном сообщении)
	bool		SMSsend(String,      char*, uint16_t=0, uint8_t=1, uint8_t=1);			      //	Объявляем  функцию для отправки SMS							(аргумент функции: текст,                                    номер,                                                                            идентификатор составного SMS сообщения,  количество SMS в составном сообщении, номер SMS в составном сообщении)
	bool		SMSsend( char*, String,      uint16_t=0, uint8_t=1, uint8_t=1);			      //	Объявляем  функцию для отправки SMS							(аргумент функции: текст,                                    номер,                                                                            идентификатор составного SMS сообщения,  количество SMS в составном сообщении, номер SMS в составном сообщении)
	bool		SMSsend(String,      String,      uint16_t=0, uint8_t=1, uint8_t=1);			//	Объявляем  функцию для отправки SMS							(аргумент функции: текст,                                    номер,                                                                            идентификатор составного SMS сообщения,  количество SMS в составном сообщении, номер SMS в составном сообщении)
  uint8_t CellLevel(void);
  uint8_t ErrCount;
  void    ResetMC35(void);
  void    BigDelay(void);
  bool    runUSSD(String, char*, uint32_t);




private:
	void 		Flush (void);																	                            //чистим буфер
	bool 		_begin (void);
	uint8_t 	pinPWR;
  uint8_t  PDpin;
	void* 		Port;
	bool		flgType = 0;
	char 		RXBuffer[RXBufferSize];
	bool 		FindSeq(uint8_t *, String &, uint8_t = 0);
	void		SketchCodingDetect(void);
	void		_SMSdecodGSM(char*, uint16_t, uint16_t, uint16_t=0);
	void		_SMSdecodAddr(char*, uint16_t, uint16_t);										              //	Объявляем  функцию разкодирования адреса SMS из strBuffer	(строка для адреса, количество полубайт в адресе, позиция адреса  в строке strBuffer)
	void		_SMSdecodDate(char*,uint16_t);													                  //	Объявляем  функцию разкодирования даты SMS из strBuffer		(строка для даты,                                 позиция даты    в строке strBuffer)
	void		_SMSdecod8BIT(char*, uint16_t, uint16_t);										              //	Объявляем  функцию разкодирования текста 8BIT из strBuffer	(строка для текста, количество байт в тексте,     позиция начала текста в строке strBuffer)
	void		_SMSdecodUCS2(char*, uint16_t, uint16_t);										              //	Объявляем  функцию разкодирования текста UCS2 из strBuffer	(строка для текста, количество байт в тексте,     позиция начала текста в строке strBuffer)
	uint8_t		codTXTread = GSM_TXT_UTF8;															                //	Тип кодировки строки StrIn.
	uint8_t		codTXTsend = GSM_TXT_UTF8;															                //	Тип кодировки строки StrIn.
	uint8_t		clsSMSsend = GSM_SMS_CLASS_NO;														              //	Класс отправляемых SMS сообщений.
	uint16_t	_SMStxtLen(char*);															                        //	Объявляем  функцию получения количества символов в строке	(строка с текстом)
	void		_SMScoderAddr(char*, uint8_t *);
	uint16_t	_SMScoderGSM( char*, uint16_t, uint16_t=255, uint8_t * = 0);						//	Объявляем  функцию кодирования текста в GSM в strBuffer		(строка с текстом,  позиция взятия из строки,     количество кодируемых символов)		Функция возвращает позицию после последнего закодированного символа из строки txt.
	uint16_t 	_SMScoderUCS2(char* , uint16_t , uint16_t , uint8_t *);
  uint8_t   _num (char);                                                            //  Объявляем  функцию преобразования символа в число     (аргумент функции: символ 0-9,a-f,A-F)
  char    _char(uint8_t);

};

#endif /* MC35I_H_ */
