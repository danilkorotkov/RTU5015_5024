#include "MC35i.h"
#include "defs.h"
#include "protocol5015.h"
#include "actions.h"
#include <EEPROM.h>
#include <Wire.h>

char IncomingPhone[PhoneLen];                 // Объявляем строку (массив) для получения адреса отправителя SMS
char SMStxt[SMStxtLen];                       // Объявляем строку (массив) для получения текста SMS.
uint8_t PhoneComp[6];                         //сжатый номер

unsigned long ResetPeriod   = 120000; // 2 минуты на ресет пароля
bool          ResetTimeout  = false;            // время на ресет истекло true / не истекло false
unsigned long PollStart     = 0;   // опрос
unsigned long PollTimeout   = 3000;   // опрос

MC35i gsm(IGT, PD);

SetsStruct Settings;

void SetupPins(void) {
  // Настраиваем пины в OUTPUT {RelayLedPin, RedPin, GreenPin, Relay1Pin, Relay2Pin, IGT, PD, PowerLedPin};
    pinMode(RelayLedPin, OUTPUT); pinMode(RedPin, OUTPUT);      pinMode(GreenPin, OUTPUT); pinMode(Relay1Pin, OUTPUT);
    pinMode(Relay2Pin, OUTPUT);   pinMode(PowerLedPin, OUTPUT); pinMode(IGT, OUTPUT);       pinMode(PD, OUTPUT);        
    
    digitalWrite(RelayLedPin, LOW); digitalWrite(RedPin, LOW);       digitalWrite(GreenPin, LOW); digitalWrite(Relay1Pin, LOW);
    digitalWrite(Relay2Pin, LOW);   digitalWrite(PowerLedPin, HIGH); digitalWrite(IGT, LOW);      digitalWrite(PD, LOW);       

    //pinMode(Input1, INPUT); pinMode(Input2, INPUT); 
    //digitalWrite(Input1, HIGH);   digitalWrite(Input2, HIGH);
    asm("WDR");
    WDTCR |= (1<<WDP2) | (1<<WDP1) | (1<<WDP0) | (1<<WDE);
}

void setup() {
    SetupPins();

    while(!gsm.begin(Serial)) {delay(50);asm("WDR");}

    //gsm.begin(Serial);
    
    EEPROM.get(0, Settings);
    asm("WDR");

    Timer1Init();
    digitalWrite(RedPin, HIGH);        // Красный до таймаута ресета

    /*while(gsm.status()!=GSM_OK) {
      // Ждём завершения регистрации модема в сети оператора связи.
      if(digitalRead(RedPin)) {digitalWrite(RedPin, LOW);}
      else {digitalWrite(RedPin, HIGH);}
      delay(100);
    }*/
    Wire.begin(AT24C32);
    PollStart = millis();

}

void loop() {
  if ( (millis() - PollStart) > PollTimeout ) { // (PollTimeout < ( millis() - PollStart ))
    PollStart = millis();
    asm("WDR");
    if(gsm.CallAvailable(IncomingPhone)){                  // Функция CALLavailable() возвращает true если есть входящий дозванивающийся вызов.
      gsm.CallEnd(); asm("WDR");
      CheckNum(IncomingPhone, SMStxt); 
    }
    asm("WDR");
    
   if(gsm.SmsAvailable()){
      gsm.SMSread(SMStxt, IncomingPhone);
      CheckSMS(SMStxt);
      if ( SMStxt[0] != 'E' && IncomingPhone[0] != 'E'){gsm.SMSsend(SMStxt, IncomingPhone);}
    }    
  }else {asm("WDR");}
    
  if (!ResetTimeout) {                                    // Отключаем сброс пароля
    if (millis() > ResetPeriod) {
        ResetTimeout = true;
        digitalWrite(RedPin, LOW);
        digitalWrite(GreenPin, HIGH);
      }
  }
}
