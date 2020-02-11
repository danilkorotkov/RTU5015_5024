/*
 * MC35i.cpp
 *
 *  Created on: 25 дек. 2019 г.
 *      Author: proizvodstvoograzdenij
 */

#include "MC35i.h"
#include "defs.h"
//#include "SoftwareSerial.h"
//#include "MemoryFree.h"


		MC35i::MC35i(uint8_t pin1, uint8_t pin2) {pinPWR=pin1; PDpin = pin2;}

bool	MC35i::_begin(){
			bool connected = false;
			//старт модуля
			pinMode(pinPWR, OUTPUT);
			digitalWrite(pinPWR, HIGH);
      asm("WDR");
			delay(1000);
      asm("WDR");
      delay(1000);
			digitalWrite(pinPWR, LOW);
      asm("WDR");
      delay(1000);
      asm("WDR");
      delay(1000);

			//if(flgType) {(*(SoftwareSerial*)Port).end();}
			//else		{(*(HardwareSerial*)Port).end();}
      (*(HardwareSerial*)Port).end();
			delay(200);asm("WDR");

			//if(flgType) {(*(SoftwareSerial*)Port).begin(GSM_UART_SPEED);}
			//else		{(*(HardwareSerial*)Port).begin(GSM_UART_SPEED);}
      (*(HardwareSerial*)Port).begin(GSM_UART_SPEED);

			for(uint8_t i=0; i<10; i++){
				if (SendATCommand(F("AT\r\n"))){i=10; connected = true;}
				else {delay(200);asm("WDR");}
			}
			if(!connected){return connected;}

			SendATCommand(F("ATE0\r\n"));                  // эхо откл
			SendATCommand(F("ATV1\r\n"));                  // ответ текстом
			SendATCommand( F("AT+CMEE=1\r\n") );             // коды ошибок
			SendATCommand(F("AT+CREG=0\r\n"));             // Отключить незапрашиваемые сообщения о статусе регистрации в сети.
			//SendATCommand(F("ATI\r\n"));                 // название и версия модуля
      //simcard booting
			BigDelay();
			SendATCommand(F("AT+CNMI=1,1,2\r\n"));         // Настройка уведомляния линии об СМС
			SendATCommand(F("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n")); //все храним на симкарте

			BigDelay(); //simcard booting
			while (SMStot == 0){
				SMStot = SMStotal();
			  delay(100);
			  asm("WDR");
			}

			SketchCodingDetect();
      
			return connected;

}

void MC35i::BigDelay(void){
            asm("WDR");
      delay(1000);
            asm("WDR");
      delay(1000);
            asm("WDR");
      delay(1000);
            asm("WDR");
      delay(1000);
}

void 	MC35i::Flush(void){
	  	  bool flgBuff = false;
	  	  unsigned long millisEnd;             //  Устанавливаем время выхода из режима ожидания завершения незапрашиваемых кодов от модуля.
	  	  do{
	  		  flgBuff = false;
	  		  millisEnd = millis() + 200;
	  		  	  while(millis()<millisEnd){    //  Пока указанное время не достигнуто ...
	  		  		  //if(flgType)	{if((*(SoftwareSerial*)Port).available()>0) {(*(SoftwareSerial*)Port).read(); flgBuff=true;}}
	  		  		  //else			{if((*(HardwareSerial*)Port).available()>0) {(*(HardwareSerial*)Port).read(); flgBuff=true;}}
	  		  		  if((*(HardwareSerial*)Port).available()>0) {(*(HardwareSerial*)Port).read(); flgBuff=true;asm("WDR");}
	  		  		  if(flgBuff)  {millisEnd=millis();asm("WDR");}
	  		  	  }
	  	  } while(flgBuff);
		}

bool 	MC35i::SendATCommand(String cmd, bool notwaiting, long tout) {
			bool _resp = false;                                       // Переменная для хранения результата
			long _timeout = millis() + tout;                          // Переменная для отслеживания таймаута (5 секунд)
			uint8_t j = 0;
			uint8_t cnt = 0;
			char i;

			Flush();

			//if(flgType)	{(*(SoftwareSerial*)Port).print(cmd);}
			//else		{(*(HardwareSerial*)Port).print(cmd);}
      (*(HardwareSerial*)Port).print(cmd);

			while ( (millis() < _timeout) ){
					_resp=false;
					if ( j >= (RXBufferSize-1)) {_timeout=millis();}
					else{

						//if(flgType)	{if((*(SoftwareSerial*)Port).available()>0) {RXBuffer[j] = (*(SoftwareSerial*)Port).read();  i = RXBuffer[j]; j++; _resp = true; } else {delay(50);}}
						//else 		{if((*(HardwareSerial*)Port).available()>0) {RXBuffer[j] = (*(HardwareSerial*)Port).read();  i = RXBuffer[j]; j++; _resp = true; } else {delay(50);}}
            if((*(HardwareSerial*)Port).available()>0) {RXBuffer[j] = (*(HardwareSerial*)Port).read();  i = RXBuffer[j]; j++; RXBuffer[j]=0; _resp = true; } else {delay(50);asm("WDR");}
					}
					//выход без таймаута
					if(notwaiting){
						if(_resp){
							if(cnt==1) {if(i=='\n'){cnt++; }                        else{cnt=0;}}else                             //  Если после 1 совпадения пришел символ '\n', то считаем его 2 символом совпавшим со строкой "\r\nOK\r\n".
							if(cnt==2) {if(i== 'O'){cnt++; }                        else{cnt=0;}}else                             //  Если после 2 совпадений пришел символ  'O', то считаем его 3 символом совпавшим со строкой "\r\nOK\r\n".
							if(cnt==3) {if(i== 'K'){cnt++; }                        else{cnt=0;}}else                             //  Если после 3 совпадений пришел символ  'K', то считаем его 4 символом совпавшим со строкой "\r\nOK\r\n".
							if(cnt==4) {if(i=='\r'){cnt++; }                        else{cnt=0;}}else                             //  Если после 4 совпадений пришел символ '\r', то считаем его 5 символом совпавшим со строкой "\r\nOK\r\n".
							if(cnt==5) {if(i=='\n'){cnt=10; ErrCount=0;}            else{cnt=0;}}else                             //  Если после 5 совпадений пришел символ '\n', значит в ответе есть "\r\nOK\r\n", выходим из цикла.
							if(cnt==6) {                         if(i=='R'){cnt++; }else{cnt=0;}}else                             //  Если после 1 совпадения пришел символ  'R', то считаем его 2 символом совпавшим со строкой "ERROR"
							if(cnt==7) {                         if(i=='R'){cnt++; }else{cnt=0;}}else                             //  Если после 2 совпадений пришел символ  'R', то считаем его 3 символом совпавшим со строкой "ERROR"
							if(cnt==8) {                         if(i=='O'){cnt++; }else{cnt=0;}}else                             //  Если после 3 совпадений пришел символ  'O', то считаем его 4 символом совпавшим со строкой "ERROR"
							if(cnt==9) {                         if(i=='R'){cnt=10; _resp = false;}else{cnt=0;}}                  //  Если после 4 совпадений пришел символ  'R', значит в ответе есть "ERROR", выходим из цикла.
							if(cnt==0) {if(i=='\r'){cnt=1; }else if(i=='E'){cnt=6; }else{cnt=0;}}                                 //  Если при отсутствии совпадений пришел символ '\r' или 'E', то считаем его 1 символом совпавшим со строкой "\r\nOK\r\n" или "ERROR".
							if(cnt==10){_timeout=millis();}
						}
					}
			}
			asm("WDR");
			if (_resp == false) {ErrCount++;}
			if (ErrCount==10){ResetMC35();}
			return _resp;
			}

void  MC35i::ResetMC35(void){
      ErrCount=0;
      //pinMode(PDpin, OUTPUT);
      digitalWrite(PDpin, HIGH);
      BigDelay();
      digitalWrite(PDpin, LOW);
      _begin();
  
}
  
uint8_t MC35i::SMStotal(void){
		  uint8_t j = 0;
		  uint8_t k = 0;
		  uint8_t i = 0;
		  bool found = false;
		  String Sequence = F("+CPMS:");

		  SendATCommand(F("AT+CPMS?\r\n"));
		  // ищем +CPMS:
		  found = FindSeq(&i, Sequence);

		  if(!found) {return 0;}

		  found = false;
		  while ( (i < RXBufferSize) && (j < 8) ) {
  			if(RXBuffer[i] == ',') {j++;}
  			if(j==8){k=i+1; found = true;}
  			i++;
		  }
		  if(!found) {return 0;}
      
		  j = _num(RXBuffer[k]);
		  k++;
		  if( RXBuffer[k]!=','  ) {j*=10; j+= _num(RXBuffer[k]);}                                      //  Если за первой цифрой не стоит знак запятой, значит там вторая цифра числа, учитываем и её.
		  asm("WDR"); 
		  return j;
		}

bool 	MC35i::FindSeq(uint8_t *index, String &Sequence, uint8_t from){
		  uint8_t i = from;
		  uint8_t j = 0;
		  bool found = false;
		  while (i < RXBufferSize && !found) {
			if(RXBuffer[i] == Sequence[j]) {
			  i++; j++; asm("WDR");
			  if ( j == Sequence.length() ) {found = true;}
			} else {i++; j = 0;}
		  }
		  *index = i;
		  return found;
		}

uint8_t MC35i::_num(char symbol){                                                          //  Аргументы функции:  символ 0-9,a-f,A-F.
			int8_t i = uint8_t(symbol);                                                          //  Получаем код символа
			if ( (i>=0x30) && (i<=0x39) ) { return i-0x30; }else                                              //  0-9
			if ( (i>=0x41) && (i<=0x46) ) { return i-0x37; }else                                              //  A-F
			if ( (i>=0x61) && (i<=0x66) ) { return i-0x57; }else                                              //  a-f
										  { return      0; }                                                //  остальные символы вернут число 0.
		}

char 	MC35i::_char(uint8_t num){                                                         //  Аргументы функции:  число 0-15.
		  if(num<10){return char(num+0x30);}else                                                      //  0-9
		  if(num<16){return char(num+0x37);}else                                                      //  A-F
					{return '0';}
		}

void 	MC35i::SketchCodingDetect(void){
		  const char* str = "п";
		  if(strlen(str)==2)              {codTXTsend=GSM_TXT_UTF8;}                                     //  Если символ 'п' занимает 2 байта, значит текст скетча в кодировке UTF8.
		  else  if(uint8_t(str[0])==0xAF) {codTXTsend=GSM_TXT_CP866;}                                     //  Если символ 'п' имеет код 175, значит текст скетча в кодировке CP866.
		  else  if(uint8_t(str[0])==0xEF) {codTXTsend=GSM_TXT_WIN1251;}                                     //  Если символ 'п' имеет код 239, значит текст скетча в кодировке WIN1251.
		}

uint8_t	MC35i::Status(void){																															////Статус сим/сети
			uint8_t i = 0;
			uint8_t j = 0;//	Объявляем временную переменную.
			String Sequence = F("+CPAS: ");

			SendATCommand(F("AT+CPAS\r\n"));																											//	Выполняем команду получения статуса модуля.

			if (!FindSeq(&i, Sequence))							{return GSM_REG_NO;			}

			if ( RXBuffer[i] =='1' ||  RXBuffer[i+1] =='1')		{return GSM_UNAVAILABLE;	}															//	Если статус модуля равен 1 значит он недоступен и AT-команды не выполняются, хотя мне интересно, а как он ответит?)))
			if ( RXBuffer[i] =='2' ||  RXBuffer[i+1] =='2')		{return GSM_UNKNOWN;		}															//	Если статус модуля равен 2 значит он неизвестен и корректное выполнение AT-команд не гарантируется.
			if ( RXBuffer[i] =='5' ||  RXBuffer[i+1] =='5')		{return GSM_SLEEP;			}															//	Если статус модуля равен 5 значит он находится в режиме ограниченной функциональности.

			SendATCommand(F("AT+CPIN?\r\n"));

			i=0; Sequence = F("+CPIN: SIM PIN");																										//	Выполняем команду проверки аутентификации SIM-карты.
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_PIN;		} 															//	Требуется ввод PIN1 (CHV1).

			i=0; Sequence = F("+CPIN: SIM PUK");
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_PUK;		}															//	Требуется ввод PUK1 и новый PIN1.

			i=0; Sequence = F("+CPIN: SIM PIN2");
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_PIN2;		}															//	Требуется ввод PIN2 (CHV2).

			i=0; Sequence = F("+CPIN: SIM PUK2");
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_PUK2;		}															//	Требуется ввод PUK2 и новый PIN2.

			i=0; Sequence = F("+CME ERROR: 10");
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_NO;			}															//	Нет сим карты.

			i=0; Sequence =F("+CME ERROR: 13");
			if ( FindSeq(&i, Sequence))							{return GSM_SIM_FAULT;		}															//	SIM-карта неисправна.

			i=0; Sequence =F("+CPIN: READY");
			if ( !FindSeq(&i, Sequence))						{return GSM_SIM_ERR;		}															//	Неопределённое состояние SIM-карты.

			SendATCommand(F("AT+CREG?\r\n"));																														//	Выполняем команду проверки статуса регистрации в сети сотового оператора.

			i=0; Sequence =F("+CREG:");
			if ( !FindSeq(&i, Sequence))						{return GSM_REG_ERR;		}															//	Статус регистрации в сети неизвестен, так как в ответе нет текста "+CREG:".

			Sequence =F(",");
			FindSeq(&i, Sequence);

			if ( RXBuffer[i+1] 		!= 13         )		{return GSM_REG_NO;			}															//	Статус регистрации модема в сети неизвестен, так как параметр статус является двухзначным числом.
			if ( RXBuffer[i] 		=='0'           )		{return GSM_REG_NO;			}															//	Если параметр статус равен 0, значит модем не зарегистрирован в сети и поиск оператора не выполняется.
			if ( RXBuffer[i] 		=='2'           )		{return GSM_REG_NO;			}															//	Если параметр статус равен 2, значит модем не зарегистрирован в сети, но выполняется поиск оператора.
			if ( RXBuffer[i] 		=='3'           )		{return GSM_REG_FAULT;		}															//	Если параметр статус равен 3, значит оператор отклонил регистрацию модема в сети.
			if(( RXBuffer[i] 		!='1'           )&&																								//	Если параметр статус не равен 1 (зарегистрирован в сети домашнего оператора) и ...
			   ( RXBuffer[i] 		!='5'           ))	{return GSM_REG_NO;			}															//	Если параметр статус не равен 5 (зарегистрирован в роуменге), то статус регистрации модема в сети неизвестен.
																return GSM_OK;																			//	Возвращаем положительный статус состояния модуля.
}

bool 	MC35i::CallAvailable(char* IncomingPhone){
		  uint8_t i = 0;
		  uint8_t j = 0;
		  uint8_t l = 0;
		  bool found = false;
		  String Sequence = F("+CLCC:");

		  SendATCommand(F("AT+CLCC\r\n"));  // cписок вызовов
		  //memset(IncomingPhone, 0, sizeof(IncomingPhone));
      IncomingPhone[0]=0;

		  // ищем +CLCC:
		  found = FindSeq(&i, Sequence);

		  if(!found) {return false;}
		  found = false;

		  //первая запятая
		  while (i < RXBufferSize && !found) {
			if(RXBuffer[i] == ',') {found = true;}
			i += 1;
			}

		  if(!found) {return false;}
		  found = false;

		  //вторая запятая
		  while (i < RXBufferSize && !found) {
			if(RXBuffer[i] == ',') {found = true;}
			i += 1;
			}

      asm("WDR");
		  if(!found) {return false;}
		  found = false;

		  //4=входящий
		  if (RXBuffer[i] == '4'){
			//начало номера
			while (i < RXBufferSize && !found) {
			  if(RXBuffer[i] == '"') {j = i+1; found = true;}
			  i += 1;
			}
			if(!found) {return false;}

      asm("WDR");

			//конец номера
			found = false;
			while (i < RXBufferSize && !found) {
			  if(RXBuffer[i] == '"') {found = true;}
			  i += 1;
			}
			if(!found) {return false;}
			i--;

			while( l < (i-j) ){
			  if (l < (PhoneLen - 1)){IncomingPhone[l] = RXBuffer[j+l];}
			  l +=1;asm("WDR");
			}
			IncomingPhone[l] = '\0';
			return true;
		  }
		  return false;
}

bool 	MC35i::SmsAvailable(void){
			uint8_t j = 0;
			uint8_t k = 0;
			uint8_t i = 0;
			bool found = false;
			String Sequence = F("+CPMS:");

			SendATCommand(F("AT+CPMS?\r\n"));
			// ищем +CPMS:
			found = FindSeq(&i, Sequence);

			if(!found) {return false;}

			found = false;
      
			while ( i < RXBufferSize && (j < 7) ) {
				asm("WDR");
				if(RXBuffer[i] == ',') {j++;} if(j==7){k=i+1; found = true;}
				i++; 
			}
      
			if(!found) {return false;}

			j = _num(RXBuffer[k]); k++;
			if( RXBuffer[k]!=','  ) {j*=10; j+= _num(RXBuffer[k]);}                                      //  Если за первой цифрой не стоит знак запятой, значит там вторая цифра числа, учитываем и её.

			SMSsum = j;
			if (SMSsum == 0) {return false;}

			return true;
}

void	MC35i::CallEnd(void){																														//	Аргументы функции:	отсутствуют.
			SendATCommand(F("AT+CHUP\r\n"));																														//	Выполняем команду сброса всех вызовов.
}

uint8_t  MC35i::CellLevel(void){                                                           //  Аргументы функции:  отсутствуют.
      uint8_t i = 0;
      String Sequence = F("+CSQ: ");
      if (SendATCommand(F("AT+CSQ\r\n"))){
        if ( FindSeq(&i,Sequence) ){
          if (RXBuffer[i+1] != ',') {return _num(RXBuffer[i])*10 + _num(RXBuffer[i+1]); }
          else {return _num(RXBuffer[i]); }
        }
      }
      return 0;                                                            
}

bool  MC35i::runUSSD(String command, char* _txt, uint32_t timeout){                                              //  Аргументы функции:  command - строка с USSD-командой,
//      Объявляем временные переменные:                                                               //  timeout - таймаут в миллисекундах,  
      uint8_t i;                                                                                          //  Переменная для хранения найденных позиций в ответе.
      char j;                                                                                         //  Переменная для хранения очередного принятого символа из строки ответа.
      bool f;                                                                                         //  Флаг указывающий на то, что принят очередной символ  из строки ответа.
      uint8_t cnt = 0;                                                                                //  Счетчик совпадний ответа со строкой "\r\nOK\r\n" или "ERROR".
      uint32_t millisEnd;                                                                             //  Переменная для хранения времени выхода из функции.
      uint16_t pos, len;                                                                              //  Переменные для хранения позиции начала и длины информационного блока в ответе (текст ответа).
      uint8_t stat, coder;                                                                            //  Переменные для хранения статуса USSD запроса и кодировки USSD ответа.
      String   Sequence;                                                                              //  

//      Определяем время выхода из функции:                                                           //  
      millisEnd = millis() + timeout;                                                                 //  Вычисляем время выхода из функции.
//      Отправляем запрос:                                                                            //  
        //(*(HardwareSerial*)Port).print((String)F("AT+CUSD=1,\"")+command+F("\",15\r\n"));           //  Выполняем команду отправки USSD запроса, модуль ответит "ЭХО\r\nOK\r\n\r\n+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n", 
        SendATCommand((String)F("AT+CUSD=1,\"")+command+F("\",15\r\n"), timeout);                     //  но функция runAT досрочно выйдет из цикла чтения после обнаружения комбинации символов "\r\nOK\r\n", 
        //SendATCommand((String)F("AT+CUSD=1,\"")+command+F("\""), timeout);                                                                                            //  значит остальная часть ответа еще будет в буфере UART.
//      Дочитываем ответ модуля в строку strBuffer:                                                   //  Дочитывается ответ содержащий строку "+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n"
      i=0;
      while(millis()<millisEnd){                                                                      //  
//        Читаем очередной символ:                                                                    //  
        f=0;
        if ( i >= (RXBufferSize-1)) {timeout=millis();}
        else{
          if((*(HardwareSerial*)Port).available()>0){
            j=(*(HardwareSerial*)Port).read(); 
            RXBuffer[i]=j; 
            i++; f=1; asm("WDR");
          }else {delay(10); asm("WDR");}}

//        Досрочно выходим из цикла получения ответа:                                                 //  Проверяем не пришли ли подряд символы "\r\nOK\r\n" или "ERROR".
        if(f){                                                                                        //
          if(cnt==0) {if(j=='\r'){cnt++;}else{cnt=0;}}else                                            //  Если при отсутствии совпадений пришел символ '\r', то считаем его 1 символом совпавшим с 1 строкой "\r\n".
          if(cnt==1) {if(j=='\n'){cnt++;}else{cnt=0;}}else                                            //  Если после 1 совпадения пришел символ '\n', то считаем его 2 символом совпавшим с 1 строкой "\r\n".
          if(cnt==2) {if(j=='\r'){cnt++;}else{cnt=2;}}else                                            //  Если после 2 совпадения пришел символ '\r', то считаем его 1 символом совпавшим с 2 строкой "\r\n".
          if(cnt==3) {if(j=='\n'){cnt++;}else{cnt=2;}}                                                //  Если после 3 совпадения пришел символ '\n', то считаем его 2 символом совпавшим с 2 строкой "\r\n".
          if(cnt==4) {millisEnd=millis();}                                                            //  Если зафиксировано 4 совпадения, значит в ответе есть две комбинации символов "\r\n", выходим из цикла.
        }                                                                                             //  
      RXBuffer[i]=0;  
      }                                                                                               //  
//      Разбираем ответ:                                                                              //  
//      i = strBuffer.indexOf( F("+CUSD:") ); if(i<0){return false;}   
//      i+=7; stat = _num(strBuffer.charAt(i));                                                       //  Получаем  позицию начала текста "+CUSD:" в ответе "ЭХО\r\nOK\r\n\r\n+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n".
      i=0;
      Sequence = F("+CUSD: "); if( !FindSeq(&i,Sequence, i) ) {return false;}
      stat = _num(RXBuffer[i]);
      
//    i = strBuffer.indexOf( '"' , i);    if(i<0){return "";}   i++;  pos = i;                      //  Сохраняем позицию первого символа текста ответа на USSD запрос.
      Sequence = F("\""); if( !FindSeq(&i, Sequence, i) ) {return false;} 
      pos = i;

//      i = strBuffer.indexOf( '"' , i);    if(i<0){return "";}       len = i-pos; len/=2;           //  Сохраняем количество байтов в тексте ответа на USSD запрос.
      if( !FindSeq(&i, Sequence, i) ) {return false;} 
      len = i-1-pos; len/=2;
      
//      i = strBuffer.indexOf( ',' , i);    if(i<0){return "";}   i++;  coder = _num(strBuffer.charAt(i)); i++;                 //  Сохраняем значение первой цифры кодировки текста.
      Sequence = F(","); if( !FindSeq(&i, Sequence, i) ) {return false;}
      coder = _num(RXBuffer[i]); i++;
      
      if( RXBuffer[i]!='\r'  ){coder*=10; coder+= _num(RXBuffer[i]); i++;}                //  Сохраняем значение второй цифры кодировки текста (если таковая существует).
      if( RXBuffer[i]!='\r'  ){coder*=10; coder+= _num(RXBuffer[i]); i++;}                //  Сохраняем значение третей цифры кодировки текста (если таковая существует).

//      Разкодируем ответ из строки strBuffer в строку _txt:                                              //
      if(coder==72){_SMSdecodUCS2(_txt, len,      pos);}else                                            //  Разкодируем ответ в строку _txt указав len - количество байт     и pos - позицию начала закодированного текста в строке strBuffer.
      if(coder==15){_SMSdecodGSM (_txt, (len/7*8),  pos);}else                                            //  Разкодируем ответ в строку _txt указав len - количество символов и pos - позицию начала закодированного текста в строке strBuffer.
                   {_SMSdecod8BIT(_txt, len,      pos);}                                              //  Разкодируем ответ в строку _txt указав len - количество байт     и pos - позицию начала закодированного текста в строке strBuffer.
//      Возвращаем ответ:                                                                             //  
                                                                                                      //  
      return(true);                                                                                   //  
} 

//		ФУНКЦИЯ ЧТЕНИЯ ВХОДЯЩЕГО SMS СООБЩЕНИЯ:																												//	Функция возвращает:	флаг успешного чтения SMS сообщения true/false
bool	MC35i::SMSread(char* txt, char* num, char* tim, uint16_t& lngID, uint8_t& lngSum, uint8_t& lngNum){													
                                                                                          //Аргументы функции:	txt - указатель для возврата текста сообщения, 
                                                                                          //num - указатель для возврата номера отправителя, 
                                                                                          //tim - указатель для возврата даты отправки сообщения, 
                                                                                          //lngID - идентификатор склеенных SMS, 
                                                                                          //lngSum - количество склеенных SMS, 
                                                                                          //lngNum - номер склеенной SMS).
//			Готовим переменные:																																//
			txt[0]=0; num[0]=0; tim[0]=0; lngID=0; lngSum=1; lngNum=1;													//	Чистим данные по ссылкам и указателям для ответа.
			uint8_t  i				      =	1;//SMSsum;																										//	В цикле do while, значение этой переменной является индексом (номером) сообщений.
			uint8_t  j				      =	0;
			uint8_t  toDel			    =	0;
			uint8_t  tooBig			    =	0;
			String 	 Sequence 		  = F("+CMGR: 0,,0");
			String 	 Sequence1 		  = F("+CMGR:");
			bool	   f			        =	false;																											//	Флаг указывающий на успешное чтение сообщения из памяти в строку strBuffer.
			uint8_t  PDU_SCA_LEN	  =	0;																													//	Первый байт блока SCA, указывает количество оставшихся байт в блоке SCA.
			uint8_t  PDU_SCA_TYPE	  =	0;																													//	Второй байт блока SCA, указывает формат адреса сервисного центра службы коротких сообщений.
			uint8_t  PDU_SCA_DATA	  =	0;																													//	Позиция первого байта блока SCA, содержащего адрес сервисного центра службы коротких сообщений.
			uint8_t  PDU_TYPE		    =	0;																													//	Первый байт блока TPDU, указывает тип PDU, состоит из флагов RP UDHI SRR VPF RD MTI (их назначение совпадает с первым байтом команды AT+CSMP).
			uint8_t  PDU_OA_LEN		  =	0;																													//	Первый байт блока OA, указывает количество полезных (информационных) полубайтов в блоке OA.
			uint8_t  PDU_OA_TYPE	  =	0;																													//	Второй байт блока OA, указывает формат адреса отправителя сообщения.
			uint8_t  PDU_OA_DATA	  =	0;																													//	Позиция третьего байта блока OA, это первый байт данных содержащих адрес отправителя сообщения.
			uint8_t  PDU_PID		    =	0;																													//	Первый байт после блока OA, указывает на идентификатор (тип) используемого протокола.
			uint8_t  PDU_DCS		    =	0;																													//	Второй байт после блока OA, указывает на схему кодирования данных (кодировка текста сообщения).
			uint8_t  PDU_SCTS_DATA  =	0;																													//	Позиция первого байта блока SCTS, содержащего дату и время отправки сообщения.
			uint8_t  PDU_UD_LEN		  =	0;																													//	Первый байт блока UD (следует за блоком SCTS), указывает на количество символов (7-битной кодировки) или количество байт (остальные типы кодировок) в блоке UD.
			uint8_t  PDU_UD_DATA	  =	0;																													//	Позиция второго байта блока UD, с данного байта начинается текст SMS или блок заголовка (UDH), зависит от флага UDHI в байте PDUT (PDU_TYPE).
			uint8_t  PDU_UDH_LEN	  =	0;																													//	Первый байт блока UDH, указывает количество оставшихся байт в блоке UDH. (блок UDH отсутствует если сброшен флаг UDHI в байте PDU_TYPE).
			uint8_t  PDU_UDH_IEI	  =	0;																													//	Второй байт блока UDH является первым байтом блока IEI, указывает на назначение заголовка. Для составных SMS значение IEI равно 0x00 или 0x08. Если IEI равно 0x00, то блок IED1 занимает 1 байт, иначе IED1 занимает 2 байта.
			uint8_t  PDU_UDH_IED_LEN=	0;																													//	Первый байт после блока IEI, указывает на количество байт в блоке IED состояшем из IED1,IED2,IED3. Значение данного байта не учитывается в данной библиотеке.
			uint16_t PDU_UDH_IED1	  =	0;																													//	Данные следуют за байтом IEDL (размер зависит от значения PDU_IEI). Для составных SMS значение IED1 определяет идентификатор длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).
			uint8_t  PDU_UDH_IED2	  =	1;																													//	Предпоследний байт блока UDH. Для составных SMS его значение определяет количество SMS в составе длинного сообщения.
			uint8_t  PDU_UDH_IED3	  =	1;																													//	Последний байт блока UDH. Для составных SMS его значение определяет номер данной SMS в составе длинного сообщения.
//			Пытаемся прочитать одно SMS сообщение из указанной области памяти в строку strBuffer:															//
      do{  if(i>SMStot){i=1;}
        asm("WDR");
        SendATCommand((String)F("AT+CMGR=")+i+F("\r\n"));                                   //  Выполняем команду чтения одного SMS сообщения с индексом i. На выполнение команды выделяем до 10 секунд.        
        if( !FindSeq(&j,Sequence) )
          {j=0;          
          if( FindSeq(&j,Sequence1) ){                                                      //  Если в строке ответа strBuffer встретился текст "+CMGR:",
            toDel = i; f=true;  //SMSsum=i;                                                   // значит в этой строке находятся требуемые данные, выходим из цикла do while и устанавливаем флаг f.
          }
        }                                                                                   //  Входим в цикл do while, пока не будет прочитано SMS сообщение или пока не будут пройдены все доступные ячейки памяти SIM-карты, где хранятся SMS сообщения.
        i++;
      } while( !f ); //while( i!=SMSsum ); 
//			Определяем значения из PDU блоков SMS сообщения находящегося в строке strBuffer:																              //
//			|                                                  PDU                                                   |										//
//			+------------------+-------------------------------------------------------------------------------------+										//
//			|                  |                                        TPDU                                         |										//
//			|                  +-----------------------------------------+-------------------------------------------+										//
//			|        SCA       |                                         |                    UD                     |										//
//			|                  |      +---------------+                  |     +--------------------------------+    |										//
//			|                  |      |      OA       |                  |     |             [UDH]              |    |										//
//			+------------------+------+---------------+-----+-----+------+-----+--------------------------------+----+										//
//			| SCAL [SCAT SCAD] | PDUT | OAL [OAT OAD] | PID | DCS | SCTS | UDL | [UDHL IEI IEDL IED1 IED2 IED3] | UD |										//
//																																							                                                      //
			if(f){																																			          //	Если SMS сообщение находится в строке strBuffer, то ...
				asm("WDR");                                            																			  //	Получаем позицию начала текста "+CMGR:" в ответе "ЭХО\r\n+CMGR: СТАТУС,["НАЗВАНИЕ"],РАЗМЕР\r\nPDU\r\n\r\nOK\r\n".
				i = j;																									                            //	Получаем позицию первой запятой следующей за текстом "+CMGR:", перед этой запятой стоит цифра статуса.
				Sequence = F(",");
				FindSeq(&i,Sequence, i); FindSeq(&i,Sequence, i);
				//if (RXBuffer[i+3] == '\r' || RXBuffer[i+3] == '\n' || RXBuffer[i+3] == 0) {tooBig = TOOBIG;}
        if (RXBuffer[i+3] == '\r') {tooBig = TOOBIG;}
				//if (RXBuffer[i+2] == '\r' || RXBuffer[i+2] == '\n' || RXBuffer[i+2] == 0) {tooBig = _num(RXBuffer[i])*10 + _num(RXBuffer[i+1]);}
        if (RXBuffer[i+2] == '\r') {tooBig = _num(RXBuffer[i])*10 + _num(RXBuffer[i+1]);}

				if (tooBig >= TOOBIG) {
					//SMS too big
					SendATCommand((String)F("AT+CMGD=")+toDel+F("\r\n"));															//	Функция удалит только что прочитанное сообщение.
					return f;
				}
	      
				Sequence = F("\r\n");                                                               //  Получаем позицию символов "\r\n" следующих за первой запятой, за символами "\r\n" следует блок PDU.
				FindSeq(&i,Sequence, i);

				//	SCAL (Service Center Address Length) - байт указывающий размер адреса сервисного центра коротких сообщений.									//
				PDU_SCA_LEN		= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);													//	Получаем значение  первого байта блока SCA (оно указывает на количество оставшихся байт в блоке SCA).

				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	SCAT (Service Center Address Type) - байт хранящий тип адреса сервисного центра коротких сообщений.											//
				PDU_SCA_TYPE	=_num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);													//	Получаем значение  второго байта блока SCA (тип адреса: номер, текст, ... ).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).
  
				//	SCAD (Service Center Address Date) - блок адреса сервисного центра коротких сообщений. С третьего байта начинается сам адрес.				//
				PDU_SCA_DATA	= i;																														      //	Сохраняем позицию  третьего байта блока SCA (для получения адреса в дальнейшем).
				i+= PDU_SCA_LEN*2 - 2;																														  //	Смещаем курсор на PDU_SCA_LEN байт после байта PDU_SCA_LEN.

				//	PDUT (Packet Data Unit Type) - байт состоящий из флагов определяющих тип блока PDU.															//
				PDU_TYPE		= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);														//	Получаем значение  байта PDU_TYPE (оно состоит из флагов RP UDHI SRR VPF RD MTI).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	OAL (Originator Address Length) - байт указывающий размер адреса отправителя.																//
				PDU_OA_LEN		= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);													//	Получаем значение  первого байта блока OA (оно указывает на количество полезных полубайтов в блоке OA).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	OAT (Originator Address Type) - байт хранящий тип адреса отправителя.																		//
				PDU_OA_TYPE		= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);													//	Получаем значение  второго байта блока OA (тип адреса: номер, текст, ... ).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	OAD (Originator Address Date) - адрес отправителя. С третьего байта начинается сам адрес, его размер равен PDU_OA_LEN полубайтов.			//
				PDU_OA_DATA		= i;																														      //	Сохраняем позицию  третьего байта блока OA (для получения адреса в дальнейшем).
				i+= (PDU_OA_LEN + (PDU_OA_LEN%2));																									//	Смещаем курсор на значение PDU_OA_LEN увеличенное до ближайшего чётного.

				//	PID (Protocol Identifier) - идентификатор протокола передачи данных, по умолчанию байт равен 00.											//
				PDU_PID			= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);														//	Получаем значение  байта PID (идентификатор протокола передачи данных).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	DCS (Data Coding Scheme) - схема кодирования данных (кодировка текста сообщения).															//
				PDU_DCS			= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);														//	Получаем значение  байта DCS (схема кодирования данных).
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	SCTS (Service Center Time Stam) - отметка о времени получения сообщения в сервис центр коротких сообщений.									//
				PDU_SCTS_DATA	= i;																														      //	Сохраняем позицию  первого байта блока SCTS (для получения даты и времени в дальнейшем).
				i+= 14;																																		          //	Смещаем курсор на 14 полубайт (7 байт).

				//	UDL (User Data Length) - размер данных пользователя.																						//
				PDU_UD_LEN		= _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]);													//	Получаем значение  байта UDL (размер данных пользователя). Для 7-битной кодировки - количество символов, для остальных - количество байт.
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	UD (User Data) - данные пользователя (заголовок и текст SMS сообщения).																		//
				PDU_UD_DATA		= i;																														      //	Позиция первого байта блока UD (данные). Блок UD может начинаться с блока UDH (заголовок), если установлен флаг UDHI в байте PDU_TYPE, а уже за ним будет следовать данные текста SMS.

				//	UDHL (User Data Header Length) - длина заголовока в данных пользователя.																	//
				PDU_UDH_LEN		= (PDU_TYPE & 0b01000000)? _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]) : 0;//	Получаем значение  первого байта блока UDH (оно указывает на количество оставшихся байт в блоке UDH). Блок UDH отсутствует если сброшен флаг UDHI в байте PDU_TYPE, иначе блок UD начинается с блока UDH.
				i+= 2;																																		           //	Смещаем курсор на 2 полубайта (1 байт).

				//	IEI (Information Element Identifier) - идентификатор информационного элемента.																//
				PDU_UDH_IEI		= (PDU_UDH_LEN)? _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]) : 0;			//	Получаем значение  первого байта блока IEI (блок указывает на назначение заголовка). Для составных SMS блок IEI состоит из 1 байта, а его значение равно 0x00 или 0x08. Если IEI равно 0x00, то блок IED1 занимает 1 байт, иначе IED1 занимает 2 байта.
				i+= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт).

				//	IEDL (Information Element Data Length) - длина данных информационных элементов.																//
				PDU_UDH_IED_LEN	= (PDU_UDH_LEN)? _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]) : 0;		//	Получаем значение  первого байта после блока IEI (оно указывает на количество байт в блоке IED состояшем из IED1,IED2,IED3). Значение данного байта не учитывается в данной библиотеке.
				i+= PDU_UDH_LEN*2 - 4;																														  //	Смещаем курсор к последнему байту блока UDH.

				//	IED3 (Information Element Data 3) - номер SMS в составе составного длинного сообщения.														//
				PDU_UDH_IED3	= (PDU_UDH_LEN)? _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]) : 0;			//	Получаем значение  последнего байта блока UDH. (оно указывает на номер SMS в составе составного длинного сообщения).
				i-= 2;																																		          //	Смещаем курсор на 2 полубайта (1 байт) к началу.

				//	IED2 (Information Element Data 2) - количество SMS в составе составного длинного сообщения.													//
				PDU_UDH_IED2	= (PDU_UDH_LEN)? _num(RXBuffer[i])*16 + _num(RXBuffer[i+1]) : 0;			//	Получаем значение  предпоследнего байта блока UDH. (оно указывает на количество SMS в составе составного длинного сообщения).
				i-= 2; if(PDU_UDH_IEI){i-= 2;}																											//	Смещаем курсор на 2 или 4 полубайта (1 или 2 байта) к началу.

				//	IED1 (Information Element Data 1) - идентификатор длинного сообщения.																		//
				PDU_UDH_IED1	= (PDU_UDH_IEI)? _num(RXBuffer[i])*4096 + _num(RXBuffer[i+1])*256 + _num(RXBuffer[i+2])*16 + _num(RXBuffer[i+3]) :
						_num(RXBuffer[i])*16   + _num(RXBuffer[i+1]);																	  //	Получаем значение  идентификатора длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).

//				Выполняем дополнительные преобразования значений блоков для удобства дальнейшей работы:															//
				//	Вычисляем схему кодирования данных (текста SMS сообщения):											//
				if((PDU_DCS&0xF0)==0xC0){
				  PDU_DCS=0;
				}else 
				  if((PDU_DCS&0xF0)==0xD0){
				    PDU_DCS=0;
				  }else 
				    if((PDU_DCS&0xF0)==0xE0){
				      PDU_DCS=2;
				    }else{
				      PDU_DCS=(PDU_DCS&0x0C)>>2;
				    }	                                                                              //	PDU_DCS = 0-GSM, 1-8бит, 2-UCS2.

				//	Вычисляем тип адреса отправителя:																								//
				if((PDU_OA_TYPE-(PDU_OA_TYPE%16))==0xD0){PDU_OA_TYPE=1;}else{PDU_OA_TYPE=0;}				//	PDU_OA_TYPE = 0 - адресом отправителя является номер телефона, 1 - адрес отправителя указан в алфавитноцифровом формате.

				//	Вычисляем длину адреса отправителя:																							//
				if(PDU_OA_TYPE){PDU_OA_LEN=(PDU_OA_LEN/2)+(PDU_OA_LEN/14);}												  //	PDU_OA_LEN = количество символов для адреса отправителя в алфавитноцифровом формате, количество цифр для адреса указанного в виде номера телефона отправителя.

				//	Вычисляем длину блока UDH вместе с его первым байтом:														//
				if(PDU_UDH_LEN>0){PDU_UDH_LEN++;}																										//	PDU_UDH_LEN теперь указывает не количество оставшихся байт в блоке UDH, а размер всего блока UDH (добавили 1 байт занимаемый самим байтом PDU_UDH_LEN).
//			Расшифровка SMS сообщения:																											  	//

				//	Получаем адрес отправителя.																						   				//
				if(PDU_OA_TYPE)	{_SMSdecodGSM (num, PDU_OA_LEN, PDU_OA_DATA);}											//	Если адрес отправителя указан в алфавитноцифровом формате, то декодируем адрес отправителя как текст в 7-битной кодировке из строки strBuffer в строку num.
				else			{_SMSdecodAddr(num, PDU_OA_LEN, PDU_OA_DATA);}														//	Иначе декодируем адрес отправителя как номер из строки strBuffer в строку num.

				//	Получаем дату отправки сообщения (дату получения SMS сервисным центром).																	//
				//_SMSdecodDate(tim, PDU_SCTS_DATA);																								//	В строку tim вернётся текст содержания "ДД.ММ.ГГ ЧЧ.ММ.СС".

				//	Получаем текст сообщения:																												//
				if(PDU_DCS==0){_SMSdecodGSM ( txt, PDU_UD_LEN            , PDU_UD_DATA, PDU_UDH_LEN    );}													//	Получаем текст сообщения принятого в кодировке GSM.
				if(PDU_DCS==1){_SMSdecod8BIT( txt, PDU_UD_LEN-PDU_UDH_LEN, PDU_UD_DATA+(PDU_UDH_LEN*2) );}													//	Получаем текст сообщения принятого в 8-битной кодировке.
				if(PDU_DCS==2){_SMSdecodUCS2( txt, PDU_UD_LEN-PDU_UDH_LEN, PDU_UD_DATA+(PDU_UDH_LEN*2) );}													//	Получаем текст сообщения принятого в кодировке UCS2.

				//	Возвращаем параметры составного сообщения:																			//
				if(PDU_UDH_LEN>1){																															    //
					lngID  = PDU_UDH_IED1;																													  //	Идентификатор составного сообщения.
					lngSum = PDU_UDH_IED2;																													  //	Количество SMS в составном сообщении.
					lngNum = PDU_UDH_IED3;																													  //	Номер данной SMS в составном сообщении.
				}																																			              //
				SendATCommand((String)F("AT+CMGD=")+toDel+"\r\n");                                  // Удаляем смс
			}																																				              //
			return f;																																		         
}
bool	MC35i::SMSread		(char* txt)	{uint16_t _lngID; uint8_t _lngSUM, _lngNUM; char _tim[18], _num[17];  return SMSread(txt,_num,_tim,_lngID,_lngSUM,_lngNUM);}
bool	MC35i::SMSread		(char* txt, char* num) {uint16_t _lngID; uint8_t _lngSUM, _lngNUM; char _tim[18]; return SMSread(txt, num,_tim,_lngID,_lngSUM,_lngNUM);}
bool	MC35i::SMSread		(char* txt, char* num, char* tim) {uint16_t _lngID; uint8_t _lngSUM, _lngNUM;	    return SMSread(txt, num, tim,_lngID,_lngSUM,_lngNUM);}

//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ GSM ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:														//
void	MC35i::_SMSdecodGSM(char* txt, uint16_t len, uint16_t pos, uint16_t udh_len){										//	Аргументы функции:	txt - строка для текста, len - количество символов в тексте, pos - позиция начала текста в строке strBuffer, udh_len количество байт занимаемое заголовком.
			if(udh_len>0){ len -= udh_len*8/7; if(udh_len*8%7){len--;} }																		//	Если текст начинается с заголовка, то уменьшаем количество символов в тексте на размер заголовка.
			uint8_t  valByteIn  = 0;																														            //	Определяем временную переменную для хранения значения очередного читаемого байта из строки strBuffer.
			uint16_t numByteIn  = udh_len*2;																												        //	Определяем временную переменную для хранения номера   очередного читаемого байта из строки strBuffer.
			uint8_t  numBitIn   = udh_len==0?0:(7-(udh_len*8%7));																						//	Определяем временную переменную для хранения номера   очередного бита в байте numByteIn.
			uint16_t numByteOut = 0;																														            //	Определяем временную переменную для хранения номера   очередного раскодируемого символа для строки txt.
			uint8_t  numBitOut  = 0;																														            //	Определяем временную переменную для хранения номера   очередного бита в байте numByteOut.
			while(numByteOut<len){																															            //	Пока номер очередного раскодируемого символа не станет больше объявленного количества символов.
				asm("WDR");
				valByteIn = _num(RXBuffer[pos+numByteIn])*16 + _num(RXBuffer[pos+numByteIn+1]); numByteIn+=2;	//	Читаем значение байта с номером numByteIn в переменную valByteIn.
				while(numBitIn<8){																															              //	Проходим по битам байта numByteIn (номер бита хранится в numBitIn).
					asm("WDR");
					bitWrite( txt[numByteOut], numBitOut, bitRead(valByteIn,numBitIn) ); numBitOut++;					  //	Копируем бит из позиции numBitIn байта numByteIn (значение valByteIn) в позицию numBitOut символа numByteOut, увеличивая значение numBitOut после каждого копирования.
					if(numBitOut>=7){ txt[numByteOut]&=0x7F; numBitOut=0; numByteOut++;}										    //	При достижении numBitOut позиции старшего бита в символе (numBitOut>=7), обнуляем старший бит символа (txt[numByteOut]&=0x7F), обнуляем позицию бита в символе (numBitOut=0), переходим к следующему символу (numByteOut++).
					numBitIn++;																																                  //
				}	numBitIn=0;																																                  //
			}		txt[numByteOut]=0;																														              //	Присваиваем символу len+1 значение 0 (конец строки).
}

//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ АДРЕСА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																					//
void	MC35i::_SMSdecodAddr(char* num, uint16_t len, uint16_t pos){																					//	Аргументы функции:	num - строка для получения адреса.
			uint8_t j=0;																																		//						len - количество полубайт в адресе (количество символов в номере).
			for(uint16_t i=0; i<len; i+=2){																														//						pos - позиция адреса в строке strBuffer.
				asm("WDR");
				if( (RXBuffer[pos+i+1]!='F') && (RXBuffer[pos+i+1]!='f') ){num[i]=RXBuffer[pos+i+1]; j++;}								//	Сохраняем следующий символ из строки strBuffer на место текущего   в строке num, если этот символ не 'F' или 'f'.
				if( (RXBuffer[pos+i  ]!='F') && (RXBuffer[pos+i  ]!='f') ){num[i+1]=RXBuffer[pos+i]; j++;}								//	Сохраняем текущий   символ из строки strBuffer на место следующего в строке num, если этот символ не 'F' или 'f'.
			}	num[j]=0;																																		//
}

//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ ДАТЫ ОТПРАВКИ SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	MC35i::_SMSdecodDate(char* tim, uint16_t pos){																								//	Аргументы функции:	tim - строка для даты
			tim[ 0]=RXBuffer[pos+5];																												//	ст. день.			pos - позиция даты в строке strBuffer
			tim[ 1]=RXBuffer[pos+4];																												//	мл. день.
			tim[ 2]='.';																																	//
			tim[ 3]=RXBuffer[pos+3];																												//	ст. мес.
			tim[ 4]=RXBuffer[pos+2];																												//	мл. мес.
			tim[ 5]='.';																																	//
			tim[ 6]=RXBuffer[pos+1];																												//	ст. год.
			tim[ 7]=RXBuffer[pos+0];																												//	мл. год.
			tim[ 8]=' ';																																	//
			tim[ 9]=RXBuffer[pos+7];																												//	ст. час.
			tim[10]=RXBuffer[pos+6];																												//	мл. час.
			tim[11]=':';																																	//
			tim[12]=RXBuffer[pos+9];																												//	ст. мин.
			tim[13]=RXBuffer[pos+8];																												//	мл. мин.
			tim[14]=':';																																	//
			tim[15]=RXBuffer[pos+11];																												//	ст. сек.
			tim[16]=RXBuffer[pos+10];																												//	мл. сек.
			tim[17]=0;
}

//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ 8BIT ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	MC35i::_SMSdecod8BIT(char* txt, uint16_t len, uint16_t pos){																					//	Аргументы функции:	txt - строка для текста, len - количество байт в тексте, pos - позиция начала текста в строке strBuffer.
			uint16_t numByteIn  = 0;																														//	Определяем временную переменную для хранения номера   очередного читаемого байта из строки strBuffer.
			uint16_t numByteOut = 0;																														//	Определяем временную переменную для хранения номера очередного раскодируемого символа для строки txt.
			while(numByteOut<len){																															//	Пока номер очередного раскодируемого символа не станет больше объявленного количества символов.
				asm("WDR");
				txt[numByteOut]= _num(RXBuffer[pos+numByteIn])*16 + _num(RXBuffer[pos+numByteIn+1]); numByteIn+=2; numByteOut++;			//	Читаем значение байта с номером numByteIn в символ строки txt под номером numByteOut.
			}	txt[numByteOut]=0;																															//	Присваиваем символу len+1 значение 0 (конец строки).
}

//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ UCS2 ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	MC35i::_SMSdecodUCS2(char* txt, uint16_t len, uint16_t pos){																					//	Аргументы функции:	txt - строка для текста, len - количество байт в тексте, pos - позиция начала текста в строке strBuffer.
			uint8_t  byteThis = 0;																															//	Определяем временную переменную для хранения значения очередного читаемого байта.
			uint8_t  byteNext = 0;																															//	Определяем временную переменную для хранения значения следующего читаемого байта.
			uint16_t numIn    = 0;																															//	Определяем временную переменную для хранения номера   очередного читаемого байта.
			uint16_t numOut   = 0;																															//	Определяем временную переменную для хранения номера   очередного раскодируемого символа.
			len*=2;																																			//	Один байт данных занимает 2 символа в строке strBuffer.
			switch(codTXTread){																																//	Тип кодировки строки StrIn.
//				Преобразуем текст из кодировки UCS2 в кодировку UTF-8:																						//
				case GSM_TXT_UTF8:																															//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){                            txt[numOut]=byteNext;      numOut++;}else											//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=0xD0; numOut++; txt[numOut]=byteNext+0x80; numOut++;}else											//	Симол  'Ё'       - 04 01          =>  208 129
						if(byteNext==0x51){txt[numOut]=0xD1; numOut++; txt[numOut]=byteNext+0x40; numOut++;}else											//	Симол  'ё'       - 04 81          =>  209 145
						if(byteNext< 0x40){txt[numOut]=0xD0; numOut++; txt[numOut]=byteNext+0x80; numOut++;}else											//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  208 144 - 208 191
						                  {txt[numOut]=0xD1; numOut++; txt[numOut]=byteNext+0x40; numOut++;}												//	Симолы 'р-я'     - 04 64 - 04 79  =>  209 128 - 209 143
					}                      txt[numOut]=0;																									//
				break;																																		//

//				Преобразуем текст из кодировки UCS2 в кодировку CP866:																						//
/*				case GSM_TXT_CP866:																															//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){txt[numOut]=byteNext;      numOut++;}else																		//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=byteNext+0xEF; numOut++;}else																		//	Симол  'Ё'       - 04 01          =>  240
						if(byteNext==0x51){txt[numOut]=byteNext+0xA0; numOut++;}else																		//	Симол  'ё'       - 04 81          =>  141
						if(byteNext< 0x40){txt[numOut]=byteNext+0x70; numOut++;}else																		//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  128 - 175
						                  {txt[numOut]=byteNext+0xA0; numOut++;}																			//	Симолы 'р-я'     - 04 64 - 04 79  =>  224 - 239
					}                      txt[numOut]=0;																									//
				break;																																		//

//				Преобразуем текст из кодировки UCS2 в кодировку Windows1251:																				//
				case GSM_TXT_WIN1251:																														//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(RXBuffer[pos+numIn])*16 + _num(RXBuffer[pos+numIn+1]); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){txt[numOut]=byteNext;      numOut++;}else																		//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=byteNext+0xA7; numOut++;}else																		//	Симол  'Ё'       - 04 01          =>  168
						if(byteNext==0x51){txt[numOut]=byteNext+0x67; numOut++;}else																		//	Симол  'ё'       - 04 81          =>  184
						if(byteNext< 0x40){txt[numOut]=byteNext+0xB0; numOut++;}else																		//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  192 - 239
						                  {txt[numOut]=byteNext+0xB0; numOut++;}																			//	Симолы 'р-я'     - 04 64 - 04 79  =>  240 - 255
					}                      txt[numOut]=0;																									//
				break;	*/																																	//
			}																																				//
}

//		ФУНКЦИЯ ОТПРАВКИ SMS СООБЩЕНИЯ:																														//	Функция возвращает:	флаг успешной отправки SMS сообщения true/false
bool	MC35i::SMSsend(char* txt, char* num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum){											//	Аргументы функции:	txt - передаваемый текст, num - номер получателя, lngID - идентификатор склеенных SMS, lngSum - количество склеенных SMS, lngNum - номер данной склеенной SMS.
//			Готовим переменные:																																//
			String Sequence;
			uint8_t i 				    =	0;
			uint16_t txtLen			  =	_SMStxtLen(txt);																						//	Количество символов (а не байтов) в строке txt.
			uint8_t  PDU_TYPE		  =	lngSum>1?0x41:0x01;																					//	Первый байт блока TPDU, указывает тип PDU, состоит из флагов RP UDHI SRR VPF RD MTI (их назначение совпадает с первым байтом команды AT+CSMP). Если сообщение составное (склеенное), то устанавливаем флаг UDHI для передачи заголовка.
			uint8_t  PDU_DA_LEN		=	strlen(num); if(num[0]=='+'){PDU_DA_LEN--;}									//	Первый байт блока DA, указывает количество полезных (информационных) полубайтов в блоке DA. Так как адрес получателя указывается ввиде номера, то значение данного блока равно количеству цифр в номере.
			uint8_t  PDU_DCS		  =	0x00;																												//	Второй байт после блока DA, указывает на схему кодирования данных (кодировка текста сообщения).
			uint8_t  PDU_UD_LEN		=	0x00;																												//	Первый байт блока UD, указывает на количество символов (в 7-битной кодировке GSM) или количество байт (остальные типы кодировок) в блоке UD.
			uint16_t PDU_UDH_IED1	=	lngID;																											//	Данные следуют за байтом IEDL (размер зависит от значения PDU_IEI). Для составных SMS значение IED1 определяет идентификатор длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).
			uint8_t  PDU_UDH_IED2	=	lngSum;																											//	Предпоследний байт блока UDH. Для составных SMS его значение определяет количество SMS в составе длинного сообщения.
			uint8_t  PDU_UDH_IED3	=	lngNum;																											//	Последний байт блока UDH. Для составных SMS его значение определяет номер данной SMS в составе длинного сообщения.
			uint16_t PDU_TPDU_LEN	=	0x00;																												//	Блок TPDU включает все блоки PDU кроме блока SCA.

//			Определяем формат кодировки SMS сообщения:																				//
			for(uint8_t i=0; i<strlen(txt); i++){ if(uint8_t(txt[i])>=0x80){PDU_DCS=0x08;}asm("WDR");}		//	Если в одном из байтов отправляемого текста установлен 7 бит, значит сообщение требуется закодировать в формате UCS2
//			Определяем класс SMS сообщения;																										//
			if(clsSMSsend==GSM_SMS_CLASS_0){PDU_DCS|=0x10;}else																	//	SMS сообщение 0 класса
			if(clsSMSsend==GSM_SMS_CLASS_1){PDU_DCS|=0x11;}else																	//	SMS сообщение 1 класса
			if(clsSMSsend==GSM_SMS_CLASS_2){PDU_DCS|=0x12;}else																	//	SMS сообщение 2 класса
			if(clsSMSsend==GSM_SMS_CLASS_3){PDU_DCS|=0x13;}																			//	SMS сообщение 3 класса
//			Проверяем формат номера (адреса получателя):																			//
			
//			if(num[0]=='+'){if(num[1]!='7'){return false;}}																		//	Если первые символы не '+7' значит номер указан не в международном формате.
//			else           {if(num[0]!='7'){return false;}}																		//	Если первый символ  не  '7' значит номер указан не в международном формате.
//			Проверяем значения составного сообщения:																					//
			if(lngSum==0)    {return false;}																										//	Количество SMS в составном сообщении должно находиться в диапазоне от 1 до 255.
			if(lngNum==0)    {return false;}																										//	Номер      SMS в составном сообщении должен находиться в диапазоне от 1 до 255.
			if(lngNum>lngSum){return false;}																										//	Номер SMS не должен превышать количество SMS в составном сообщении.
//			Проверяем длину текста сообщения:																									//
			if(PDU_DCS%16==0){ if(lngSum==1){ if(txtLen>160){txtLen=160;} }else{ if(txtLen>152){txtLen=152;} } }	//	В формате GSM  текст сообщения не должен превышать 160 символов для короткого сообщения или 152 символа  для составного сообщения.
			if(PDU_DCS%16==8){ if(lngSum==1){ if(txtLen>70 ){txtLen= 70;} }else{ if(txtLen> 66){txtLen= 66;} } }	//	В формате UCS2 текст сообщения не должен превышать  70 символов для короткого сообщения или 66  символов для составного сообщения.
//			Определяем размер блока UD: (блок UD может включать блок UDH - заголовок, который так же учитывается)										
                                                                                          //	Количество байт отводимое для кодированного текста и заголовка (если он присутствует).
			if(PDU_DCS%16==0){PDU_UD_LEN=txtLen;	if(lngSum>1){PDU_UD_LEN+=8;}}									//	Получаем размер блока UD в символах, при кодировке текста сообщения в формате GSM  и добавляем размер заголовка (7байт = 8символов) если он есть.
			if(PDU_DCS%16==8){PDU_UD_LEN=txtLen*2;	if(lngSum>1){PDU_UD_LEN+=7;}}								//	Получаем размер блока UD в байтах,   при кодировке текста сообщения в формате UCS2 и добавляем размер заголовка (7байт) если он есть.
//			Определяем размер блока TPDU:																											//
			if(PDU_DCS%16==0){PDU_TPDU_LEN = 0x0D + PDU_UD_LEN*7/8; 
			if(PDU_UD_LEN*7%8){PDU_TPDU_LEN++;} }	                                              //	Размер блока TPDU = 13 байт занимаемые блоками (PDUT, MR, DAL, DAT, DAD, PID, DCS, UDL) + размер блока UD (рассчитывается из количества символов указанных в блоке UDL).
			if(PDU_DCS%16==8){PDU_TPDU_LEN = 0x0D + PDU_UD_LEN;}																//	Размер блока TPDU = 13 байт занимаемые блоками (PDUT, MR, DAL, DAT, DAD, PID, DCS, UDL) + размер блока UD (указан в блоке UDL).
//			Отправляем SMS сообщение:																													//
			
			SendATCommand( ((String)F("AT+CMGS=")+PDU_TPDU_LEN+F("\r\n")), 1000);								//	Выполняем команду отправки SMS без сохранения в область памяти, отводя на ответ до 1 сек.
//			Проверяем готовность модуля к приёму блока PDU для отправки SMS сообщения:				//

			Sequence = F("\r\n>");
			if( !FindSeq(&i, Sequence) ){ return false; }																			  //	Если модуль отказывается принять сообщение, то выходим из функции.
			
//			Создаём блок PDU в строке strBuffer:																							//
//			|                                                     PDU                                                     |									//
//			+------------------+------------------------------------------------------------------------------------------+									//
//			|                  |                                           TPDU                                           |									//
//			|                  +----------------------------------------------+-------------------------------------------+									//
//			|        SCA       |                                              |                    UD                     |									//
//			|                  |           +---------------+                  |     +--------------------------------+    |									//
//			|                  |           |      DA       |                  |     |              UDH               |    |									//
//			+------------------+------+----+---------------+-----+-----+------+-----+--------------------------------+----+									//
//			| SCAL [SCAT SCAD] | PDUT | MR | DAL [DAT DAD] | PID | DCS | [VP] | UDL | [UDHL IEI IEDL IED1 IED2 IED3] | UD |									//
//																																							//
				i = 0;
				//memset(RXBuffer, 0, sizeof(RXBuffer));																						//	Готовим строку strBuffer.
				

				RXBuffer[i] = '0';	i++; RXBuffer[i] = '0';	i++;											            // SCAL	(Service Center Address Length)			
				                                                                                  //	Байт указывающий размер адреса сервисного центра.		
				                                                                                  //Указываем значение 0x00. Значит блоков SCAT и SCAD не будет. В этом случае модем возьмет адрес сервисного центра не из блока SCA, а с SIM-карты.

				RXBuffer[i] = _char(PDU_TYPE/16);i++;		RXBuffer[i] = _char(PDU_TYPE%16); i++;		// PDUT	(Packet Data Unit Type)					
				                                                                                  //	Байт состоящий из флагов определяющих тип блока PDU.	
				                                                                                  //Указываем значение 0x01 или 0x41. RP=0 UDHI=0/1 SRR=0 VPF=00 RD=0 MTI=01. RP=0 - обратный адрес не указан, 
				                                                                                  //UDHI=0/1 - наличие блока заголовка, SRR=0 - не запрашивать отчёт о доставке, VPF=00 - нет блока срока жизни сообщения, 
				                                                                                  //RD=0 - не игнорировать копии данного сообщения, MTI=01 - данное сообщение является исходящим.

				RXBuffer[i] = '0';	i++; RXBuffer[i] = '0';	i++;											            // MR	(Message Reference)						
				                                                                                  //	Байт													
				                                                                                  //Указываем значение 0x00.

				RXBuffer[i] = _char(PDU_DA_LEN/16);i++;		RXBuffer[i] = _char(PDU_DA_LEN%16);i++;	// DAL	(Destination Address Length)			
				                                                                                  //Байт указывающий размер адреса получателя.				
				                                                                                  //Указываем значение количество цифр в номере получател (без знака + и т.д.). +7(123)456-78-90 => 11 цифр = 0x0B.

				RXBuffer[i] = '9';	i++; RXBuffer[i] = '1';	i++;											            // DAT	(Destination Address Type)				
				                                                                                  //	Байт хранящий тип адреса получателя.					
				                                                                                  //Указываем значение 0x91. Значит адрес получателя указан в международном формате: +7******... .

				_SMScoderAddr(num, &i);																		                        // DAD	(Destination Address Date)				
				                                                                                  //	Блок с данными адреса получателя.						
				                                                                                  //Указываем номер num, кодируя его в конец строки strBuffer.

				RXBuffer[i] = '0';	i++; RXBuffer[i] = '0';	i++;											            // PID	(Protocol Identifier)					
				                                                                                  //	Байт с идентификатором протокола передачи данных.		
				                                                                                  //Указываем значение 0x00. Это значение по умолчанию.

				RXBuffer[i] = _char(PDU_DCS/16); i++;		RXBuffer[i] = _char(PDU_DCS%16); i++;			// DCS	(Data Coding Scheme)					
				                                                                                  //	Байт указывающий схему кодирования данных.				
				                                                                                  //Будем использовать значения 00-GSM, 08-UCS2 и 10-GSM, 18-UCS2. 
				                                                                                  //Во втором случае сообщение отобразится на дисплее, но не сохраняется на телефоне получателя.

				RXBuffer[i] = _char(PDU_UD_LEN/16);	i++;	RXBuffer[i] = _char(PDU_UD_LEN%16);	i++;// UDL	(User Data Length)						
				RXBuffer[i] = 0;                                                                  //	Байт указывающий размер данных (размер блока UD).		
				                                                                                  //Для 7-битной кодировки - количество символов, для остальных - количество байт.

			if(lngSum>1){																				                                	// UDH	(User Data Header)						
			                                                                                      //Если отправляемое сообщение является стоставным,		
			                                                                                      //то добавляем заголовок (блок UDH)...
				RXBuffer[i] = '0';	i++; RXBuffer[i] = '6';	i++;											              // UDHL	(User Data Header Length)				
				                                                                                    //	Байт указывающий количество оставшихся байт блока UDH.	
				                                                                                    //Указываем значение 0x06. Это значит что далее следуют 6 байт: 
				                                                                                    //1 байт IEI, 1 байт IEDL, 2 байта IED1, 1 байт IED2 и 1 байт IED3.
				RXBuffer[i] = '0';	i++; RXBuffer[i] = '8';	i++;											              // IEI	(Information Element Identifier)		
				                                                                                    //	Байт указывающий на назначение заголовка.				
				                                                                                    //Указываем значение 0x08. Это значит что данное сообщение является составным с 2 байтным блоком IED1 
				                                                                                    //(если указать значение 0x0, то блок IED1 должен занимать 1 байт).
				RXBuffer[i] = '0';	i++; RXBuffer[i] = '4';	i++;;											              // IEDL	(Information Element Data Length)		
				                                                                                    //	Байт указывающий количество оставшихся байт блока IED.	Указываем значение 0x04. 
				                                                                                    //Это значит что далее следуют 4 байта: 2 байта IED1, 1 байт IED2 и 1 байт IED3.
				RXBuffer[i] = _char(PDU_UDH_IED1/4096); i++; 
				RXBuffer[i] = _char(PDU_UDH_IED1%4096/256);i++;                                     // IED1	(Information Element Data 1)			
				                                                                                    //	Блок указывающий идентификатор составного сообщения.	
				                                                                                    //Все сообщения в составе составного должны иметь одинаковый идентификатор.
				RXBuffer[i] = _char(PDU_UDH_IED1%256/16);i++; 
				RXBuffer[i] =  _char(PDU_UDH_IED1%16);i++;	                                        //		(2 байта)								//
				
				RXBuffer[i] = _char( PDU_UDH_IED2/16);i++; 	  
				RXBuffer[i] = _char(PDU_UDH_IED2%16);i++;		                                        // IED2	(Information Element Data 1)			
				                                                                                    //	Байт указывающий количество SMS в составе составного сообщения.
				RXBuffer[i] = _char( PDU_UDH_IED3/16);i++;    
				RXBuffer[i] += _char(PDU_UDH_IED3%16);i++;	                                        // IED3	(Information Element Data 1)			
				
				RXBuffer[i] = 0;                                                                    //	Байт указывающий номер данной SMS в составе составного сообщения.
			}																								                                      // UD	(User Data)								
      asm("WDR");
			                                                                                      //	Блок данных пользователя (текст сообщения).				Указывается в 16-ричном представлении.


//			Передаём полученные блоки PDU:																											//
			SendATCommand(RXBuffer, false, 500);																									//	Выполняем отправку текста strBuffer выделяя на неё до 500 мс.
			//memset(RXBuffer, 0, sizeof(RXBuffer)); 
			i = 0;																					                                      //	Чистим строку strBuffer.

//			Кодируем и передаём текст сообщения по 24 символа:																	//	Передача текста частями снижает вероятность выхода за пределы "кучи" при передаче длинных сообщений.
			uint16_t txtPartSMS=0;																							  								//	Количество отправляемых символов за очередной проход цикла.
			uint16_t txtPosNext=0;																								  							//	Позиция после последнего отправленного символа в строке txt.
			while (txtLen) {																												      				//
				asm("WDR");
				txtPartSMS = txtLen>24 ? 24 : txtLen; 
				txtLen-=txtPartSMS;

				if(PDU_DCS%16==0){txtPosNext=_SMScoderGSM ( txt, txtPosNext, txtPartSMS, &i);}			//	Записать в конец строки strBuffer закодированный в формат GSM  текст сообщения, 
				                                                                                    //текст брать из строки txt начиная с позиции txtPosNext, 
				                                                                                    //всего взять txtPartSMS символов (именно символов, а не байтов).
				if(PDU_DCS%16==8){txtPosNext=_SMScoderUCS2( txt, txtPosNext, txtPartSMS, &i);}			//	Записать в конец строки strBuffer закодированный в формат UCS2 текст сообщения, 
				                                                                                    //текст брать из строки txt начиная с позиции txtPosNext, 
				                                                                                    //всего взять txtPartSMS символов (именно символов, а не байтов).
        RXBuffer[i] = 0;
				SendATCommand(RXBuffer, false, 500);																								//	Выполняем отправку текста strBuffer выделяя на неё до 500 мс.
				
				//memset(RXBuffer, 0, sizeof(RXBuffer)); 
				i = 0;																				                                      //	Чистим строку strBuffer.
			}																																				              //
//			Передаём байт подтверждения отправки SMS сообщения:																	//
			SendATCommand("\32",10000);																														//	Отправляем символ подтверждения отправки 26 = 0xA1 = 032 = 0b10100001. На выполнение команды выделяем до 10 секунд.

//			Проверяем факт отправки сообщения:																									//
			Sequence =F("+CMGS:" );
			if(!FindSeq(&i, Sequence)){return false;}																							//	Если модуль не вернул ответ +CMGS: X, значит SMS сообщение не отправлено, возвращаем false.
			return true;																																	        //
}																																							              //

bool	MC35i::SMSsend		(String      txt,  char* num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{char _txt[161];                txt.toCharArray(_txt, 161);                            return SMSsend(_txt,  num, lngID, lngSum, lngNum);}
bool	MC35i::SMSsend		(char* txt, String      num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{                char _num[15];                             num.toCharArray(_num, 15); return SMSsend( txt, _num, lngID, lngSum, lngNum);}
bool	MC35i::SMSsend		(String      txt, String      num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{char _txt[161]; char _num[15]; txt.toCharArray(_txt, 161); num.toCharArray(_num, 15); return SMSsend(_txt, _num, lngID, lngSum, lngNum);}

//		ФУНКЦИЯ ПОЛУЧЕНИЯ КОЛИЧЕСТВА СИМВОЛОВ В СТРОКЕ С УЧЁТОМ КОДИРОВКИ:																					//	Функция возвращает:	число uint16_t соответствующее количеству символов (а не байт) в строке.
uint16_t MC35i::_SMStxtLen(char* txt){																											//	Аргументы функции:	txt - строка с текстом
			uint16_t numIn=0, sumSymb=0;																													//	Объявляем временные переменные.
			uint16_t lenIn=strlen(txt);																														//
			uint8_t  valIn=0;																																//

			switch(codTXTsend){																																//	Тип кодировки строки txt.
			//	Получаем количество символов в строке txt при кодировке UTF-8:																				//
				case GSM_TXT_UTF8:																															//
					while(numIn<lenIn){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						valIn=uint8_t(txt[numIn]); sumSymb++;																								//
						asm("WDR");
						if(valIn<0x80){numIn+=1;}else																										//  Символ состоит из 1 байта
						if(valIn<0xE0){numIn+=2;}else																										//  Символ состоит из 2 байт
						if(valIn<0xF0){numIn+=3;}else																										//  Символ состоит из 3 байт
						if(valIn<0xF8){numIn+=4;}else																										//  Символ состоит из 4 байт
						              {numIn+=5;}																											//	Символ состоит из 5 и более байт
					}																																		//
				break;																																		//
			//	Получаем количество символов в строке txt при кодировке CP866:																				//
				case GSM_TXT_CP866:   sumSymb=lenIn; break;																									//
			//	Получаем количество символов в строке txt при кодировке Windows1251:																		//
				case GSM_TXT_WIN1251: sumSymb=lenIn; break;																									//
			}	
			return sumSymb;																																//
}

//		ФУНКЦИЯ КОДИРОВАНИЯ АДРЕСА SMS СООБЩЕНИЯ В СТРОКУ strBuffer:																						//
void	MC35i::_SMScoderAddr(char* num, uint8_t *index){																										//	Аргументы функции:	num - строка с адресом для кодирования.
		uint16_t j=num[0]=='+'?1:0, len=strlen(num);																										//	Определяем временные переменные.

		for(uint16_t i=j; i<len; i+=2){																														//
			asm("WDR");
			if( (len<=(i+1)) || (num[i+1]==0) ){RXBuffer[*index]='F'; (*index)++;} else{RXBuffer[*index]=num[i+1]; (*index)++;}									//	Сохраняем следующий символ из строки num в строку strBuffer, если символа в строке num нет, то сохраняем символ 'F'.
			if( (len<= i   ) || (num[i  ]==0) ){RXBuffer[*index]='F'; (*index)++;} else{RXBuffer[*index]=num[i  ]; (*index)++;}									//	Сохраняем текущий   символ из строки num в строку strBuffer, если символа в строке num нет, то сохраняем символ 'F'.
		}																																					//
		
}

//		ФУНКЦИЯ КОДИРОВАНИЯ ТЕКСТА SMS СООБЩЕНИЯ В СТРОКУ strBuffer В ФОРМАТЕ GSM:																			//	Функция возвращает:	число uint16_t соответствующее позиции после последнего закодированного символа из строки txt.
uint16_t MC35i::_SMScoderGSM(char* txt, uint16_t pos, uint16_t len, uint8_t *index){																			//	Аргументы функции:	txt - строка с текстом, pos - позиция взятия первого символа из строки txt, len - количество кодируемых символов из строки txt.
			uint8_t  valByteIn = 0;																															//	Определяем временную переменную для хранения значения очередного читаемого символа из строки txt.
			uint16_t numByteIn = 0;																															//	Определяем временную переменную для хранения номера   очередного читаемого символа из строки txt.
			uint8_t  numBitIn = 0;																															//	Определяем временную переменную для хранения номера   очередного бита в байте valByteIn.
			uint8_t  valByteOut = 0;																														//	Определяем временную переменную для хранения значения очередного сохраняемого байта в строку strBuffer.
			uint8_t  numBitOut = 0;																															//	Определяем временную переменную для хранения номера   очередного бита в байте valByteOut.
			if(len==255){len=strlen(txt)-pos;}																												//
			while( numByteIn < len ){																														//	Пока номер очередного читаемого символа меньше заданного количества читаемых символов.
				asm("WDR");
				valByteIn = txt[pos+numByteIn]; numByteIn+=1;																								//	Читаем значение символа с номером numByteIn в переменную valByteIn.
				for(numBitIn=0; numBitIn<7; numBitIn++){																									//	Проходим по битам байта numByteIn (номер бита хранится в numBitIn).
					asm("WDR");
					bitWrite( valByteOut, numBitOut, bitRead(valByteIn,numBitIn) ); numBitOut++;															//	Копируем бит из позиции numBitIn байта numByteIn (значение valByteIn) в позицию numBitOut символа numByteOut, увеличивая значение numBitOut после каждого копирования.
					if(numBitOut>7){																														//
						RXBuffer[*index] = _char(valByteOut/16); (*index)++;																									//
						RXBuffer[*index] = _char(valByteOut%16); (*index)++;																									//
						valByteOut=0; numBitOut=0;																											//
					}																																		//	При достижении numBitOut позиции старшего бита в символе (numBitOut>=7), обнуляем старший бит символа (txt[numByteOut]&=0x7F), обнуляем позицию бита в символе (numBitOut=0), переходим к следующему символу (numByteOut++).
				}																																			//
			}																																				//
			if(numBitOut){																																	//
				RXBuffer[*index] = _char(valByteOut/16); (*index)++;																											//
				RXBuffer[*index] = _char(valByteOut%16); (*index)++;																											//
			}																																				//
			return pos+numByteIn;																															//
}
																																							//
//		ФУНКЦИЯ КОДИРОВАНИЯ ТЕКСТА SMS СООБЩЕНИЯ В СТРОКУ strBuffer В ФОРМАТЕ UCS2:																			//	Функция возвращает:	число uint16_t соответствующее позиции после последнего закодированного символа из строки txt.
uint16_t MC35i::_SMScoderUCS2(char* txt, uint16_t pos, uint16_t len, uint8_t *index){																			//	Аргументы функции:	txt - строка с текстом, pos - позиция взятия первого символа из строки txt, len - количество кодируемых символов из строки txt.
			uint8_t  valByteInThis  = 0;																													//	Определяем временную переменную для хранения значения очередного читаемого байта.
			uint8_t  valByteInNext  = 0;																													//	Определяем временную переменную для хранения значения следующего читаемого байта.
			uint16_t numByteIn      = pos;																													//	Определяем временную переменную для хранения номера очередного   читаемого байта.
			uint16_t numSymbIn      = 0;																													//	Определяем временную переменную для хранения номера очередного   читаемого символа.
			uint8_t  lenTXT         = strlen(txt);																											//	Определяем временную переменную для хранения длины строки в байтах.
			switch(codTXTsend){																																//	Тип кодировки строки StrIn.
//				Преобразуем текст из кодировки UTF-8 кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)							//
				case GSM_TXT_UTF8:																															//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						asm("WDR");
						valByteInThis = uint8_t(txt[numByteIn  ]);																							//	Читаем значение очередного байта.
						valByteInNext = uint8_t(txt[numByteIn+1]);																							//	Читаем значение следующего байта.
						numSymbIn++;																														//	Увеличиваем количество прочитанных символов.
								if (valByteInThis==0x00){return numByteIn;																					//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80){
								numByteIn+=1;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;												//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
												RXBuffer[*index]=_char(valByteInThis/16);(*index)++;															//	                                                                    записываем его старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);(*index)++;															//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xD0){
								numByteIn+=2;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;												//	Очередной прочитанный символ состоит из 2 байт и является символом Русского алфавита, записываем 04 и проверяем значение байта valByteInNext ...
								if (valByteInNext==0x81){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='1';(*index)++;}												//	Симол  'Ё'       - 208 129            =>  04 01
								if((valByteInNext>=0x90)&&(valByteInNext<=0xBF)){
												RXBuffer[*index]=_char((valByteInNext-0x80)/16);(*index)++;													//	Симолы 'А-Я,а-п' - 208 144 - 208 191  =>  04 16 - 04 63
												RXBuffer[*index]=_char((valByteInNext-0x80)%16);(*index)++;}													//
						}else	if (valByteInThis==0xD1){
								numByteIn+=2;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;												//	Очередной прочитанный символ состоит из 2 байт и является символом Русского алфавита, записываем 04 и проверяем значение байта valByteInNext ...
								if (valByteInNext==0x91){
												RXBuffer[*index]='5';(*index)++;RXBuffer[*index]='1';(*index)++;}												//	Симол  'ё'       - 209 145            =>  04 81
								if((valByteInNext>=0x80)&&(valByteInNext<=0x8F)){
												RXBuffer[*index]=_char((valByteInNext-0x40)/16);(*index)++;													//	Симолы 'р-я'     - 209 128 - 209 143  =>  04 64 - 04 79
												RXBuffer[*index]=_char((valByteInNext-0x40)%16);(*index)++;}													//
						}else	if (valByteInThis <0xE0){
								numByteIn+=2;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;
												RXBuffer[*index]='3';(*index)++;RXBuffer[*index]='F';(*index)++;												//  Очередной прочитанный символ состоит из 2 байт,         записываем его как символ '?'
						}else	if (valByteInThis <0xF0){
								numByteIn+=3;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;
												RXBuffer[*index]='3';(*index)++;RXBuffer[*index]='F';(*index)++;												//  Очередной прочитанный символ состоит из 3 байт,         записываем его как символ '?'
						}else	if (valByteInThis <0xF8){
								numByteIn+=4;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;
												RXBuffer[*index]='3';(*index)++;RXBuffer[*index]='F';(*index)++;												//  Очередной прочитанный символ состоит из 4 байт,         записываем его как символ '?'
						}else							{
								numByteIn+=5;	RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;
												RXBuffer[*index]='3';(*index)++;RXBuffer[*index]='F';(*index)++;}												//	Очередной прочитанный символ состоит из 5 и более байт, записываем его как символ '?'
					}																																		//
				break;																																		//
//				Преобразуем текст из кодировки CP866 в кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)						//
/*				case GSM_TXT_CP866:																															//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						asm("WDR");
						valByteInThis = uint8_t(txt[numByteIn]);																							//	Читаем значение очередного символа.
						numSymbIn++; numByteIn++;																											//	Увеличиваем количество прочитанных символов и номер прочитанного байта.
								if (valByteInThis==0x00){return numByteIn;																					//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;												//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
												RXBuffer[*index]=_char(valByteInThis/16);(*index)++;															//	                                                                    записываем его старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);(*index)++;															//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xF0){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='1';(*index)++;												//	Симол  'Ё'       - 240            =>  04 01              	        записываем его байты.
						}else	if (valByteInThis==0xF1){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;
												RXBuffer[*index]='5';(*index)++;RXBuffer[*index]='1';(*index)++;												//	Симол  'e'       - 241            =>  04 81                         записываем его байты.
						}else	if((valByteInThis>=0x80)&&(valByteInThis<=0xAF)){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++; valByteInThis-=0x70;							//  Симолы 'А-Я,а-п' - 128 - 175      =>  04 16 - 04 63                 записываем 04 и вычисляем значение для кодировки UCS2:
												RXBuffer[*index]=_char(valByteInThis/16);(*index)++;															//	                                                                    записываем старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);(*index)++;															//	                                                                    записываем младший полубайт.
						}else	if((valByteInThis>=0xE0)&&(valByteInThis<=0xEF)){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++; valByteInThis-=0xA0;							//	Симолы 'р-я'     - 224 - 239      =>  04 64 - 04 79                 записываем 04 и вычисляем значение для кодировки UCS2:
												RXBuffer[*index]=_char(valByteInThis/16); (*index)++;															//	                                                                    записываем старший полубайт.
												RXBuffer[*index]+=_char(valByteInThis%16); (*index)++;														//	                                                                    записываем младший полубайт.
						}																																	//
					}																																		//
				break;																																		//
//				Преобразуем текст из кодировки Windows1251 в кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)					//
				case GSM_TXT_WIN1251:																														//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						asm("WDR");
						valByteInThis = uint8_t(txt[numByteIn]);																							//	Читаем значение очередного символа.
						numSymbIn++; numByteIn++;																											//	Увеличиваем количество прочитанных символов и номер прочитанного байта.
								if (valByteInThis==0x00){return numByteIn;																					//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='0';(*index)++;												//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
												RXBuffer[*index]=_char(valByteInThis/16);	(*index)++;														//	                                                                    записываем его старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);	(*index)++;														//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xA8){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='1';(*index)++;												//	Симол  'Ё'       - 168            =>  04 01              	        записываем его байты.
						}else	if (valByteInThis==0xB8){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++;
												RXBuffer[*index]='5';(*index)++;RXBuffer[*index]='1';(*index)++;												//	Симол  'e'       - 184            =>  04 81                         записываем его байты.
						}else	if((valByteInThis>=0xC0)&&(valByteInThis<=0xEF)){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++; valByteInThis-=0xB0;							//  Симолы 'А-Я,а-п' - 192 - 239      =>  04 16 - 04 63                 записываем 04 и вычисляем значение для кодировки UCS2:
												RXBuffer[*index]=_char(valByteInThis/16);																	//	                                                                    записываем старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);																	//	                                                                    записываем младший полубайт.
						}else	if((valByteInThis>=0xF0)&&(valByteInThis<=0xFF)){
												RXBuffer[*index]='0';(*index)++;RXBuffer[*index]='4';(*index)++; valByteInThis-=0xB0;							//	Симолы 'р-я'     - 240 - 255      =>  04 64 - 04 79                 записываем 04 и вычисляем значение для кодировки UCS2:
												RXBuffer[*index]=_char(valByteInThis/16);																	//	                                                                    записываем старший полубайт.
												RXBuffer[*index]=_char(valByteInThis%16);																	//	                                                                    записываем младший полубайт.
						}																																	//
					}																																		//
				break;*/																																		//
			}																																				//
			return numByteIn;																																//
}
