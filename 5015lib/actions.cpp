#include "actions.h"
#include "defs.h"
#include "m16Timer1.h"
#include <Wire.h>
#include <EEPROM.h>


void Timer1Init(){
    timer1_setPeriod( Settings.RelayOnTime * 1000000 /2 );
    //timer1_stop();
    timer1_ISR(INT_TIMER1);
}

void INT_TIMER1() {
  digitalWrite(Relay1Pin, LOW);
  digitalWrite(RelayLedPin, LOW);
}

bool Relay2(bool r, char* t){
  if (r) {digitalWrite(Relay2Pin, HIGH); strcpy_P( t, (const char *)RELAY2ON  );}
  else   {digitalWrite(Relay2Pin,  LOW); strcpy_P( t, (const char *)RELAY2OFF );}
  return true;
}

void Relay1(){
  digitalWrite(Relay1Pin, !digitalRead(Relay1Pin));
  digitalWrite(RelayLedPin, digitalRead(Relay1Pin));
  timer1_start();
}

bool ErrorF(char* t){
  strcpy_P( t, (const char *)F("Error") ); return false;
}

bool ZipNum(char* t, uint8_t *j){
  for (uint8_t k = 5; k >0; k--){

      if ( !( uint8_t(t[*j]     >=0x30)   && uint8_t(t[*j]    <=0x39) && 
              uint8_t(t[(*j)-1] >=0x30)   && uint8_t(t[(*j)-1]<=0x39) ))   { k=-1; return ErrorF(t);}
      if (k>0) {PhoneComp[k] = ( uint8_t(t[*j] - 0x30) ) + (( uint8_t( t[(*j)-1] - 0x30 )) << 4 ); *j = (*j)-2;}
      }
    
  if ( (uint8_t(t[*j]) - 0x30) >= 0x00   && (uint8_t(t[*j]) - 0x39) <=0x09){
    PhoneComp[0] = ( uint8_t(t[*j] - 0x30)   + 0xF0 );  
  } else {return ErrorF(t);}
  
  return true;
}

bool FindNum(char *t){
  uint16_t i, pos;
  uint8_t zip;
  bool found = false;
  String mess;

  for(i=0; i<680; i++){
     
    for (int8_t j=5; j>=0; j--){
      
      zip =  ReadMemory(i+j);
      if (PhoneComp[j] != zip){j=-1; found = false;}
      else {found = true;}
      if (j==0 && found){pos = (i+6)/6; i=681;}
    }
  }

  if (found){
    mess = (String)char( (pos)/100      + '0' ) + char( ((pos)%100)/10 + '0' ) + char( ((pos)%100)%10 + '0' );
    mess += MEMCELL; 
    strcpy(t, mess.c_str()); 
    
    //AddStrToChar(t, mess);
    return true;
  }else {
    strcpy_P(t, (const char *)NOTFOUND);
    return false;}
}

byte ReadMemory(word address){
    Wire.beginTransmission(AT24C32);
    Wire.write(highAddressByte(address));      //First Word Address
    Wire.write(lowAddressByte(address));      //Second Word Address
    Wire.endTransmission();
    asm("WDR");
    Wire.requestFrom(AT24C32, 1);
    asm("WDR");
    return Wire.read();
}

void ClearMemory(uint16_t value, uint16_t from){
    for(uint16_t i=0; i<value; i++){
      Wire.beginTransmission(AT24C32);
      Wire.write(highAddressByte(i+from));      //First Word Address
      Wire.write(lowAddressByte(i+from));      //Second Word Address
    
      Wire.write(0x00);      //Write an \0 
      delay(10);asm("WDR");
      Wire.endTransmission();
    }   
}

byte highAddressByte(word address){
  byte BYTE_1;
  BYTE_1 = address >> 8;
  return BYTE_1;
}

byte lowAddressByte(word address){
  byte BYTE_1;
  BYTE_1 = address & 0xFF;
  return BYTE_1;
}

bool FindSeq(uint8_t *index, String &Sequence, char * t, uint8_t from = 0){
      uint8_t i = from;
      uint8_t j = 0;
      bool found = false;
      while (t[i] && !found) {
      if(t[i] == Sequence[j]) {
        i++; j++;asm("WDR");
        if ( j == Sequence.length() ) {found = true;}
      } else {i++; j = 0;}
      }
      *index = i;
      return found;
}

bool Reset(char *t, bool r){
  if (!r) {strcpy(Settings.Pass, "1234"); EEPROM.put(0, Settings);}
  else {
    EEPROM.get(100, Settings);
    ClearMemory(4096, 0);
  }
  strcpy_P(t, (const char *)RESETED); 
  return true;
}

bool PwdChange(char* t, uint8_t *i){
  uint8_t k;
  //1234#PWD[6]666#PWD6666#
  *i -= 3;
  for (uint8_t j=0; j<8; j++){
    if ( t[(*i) + j] == t[(*i) + 8 + j] ) {asm("nop");}
    else {j=8; return ErrorF(t);}
  }
  
  *i += 3;
  for (uint8_t j=0; j<4; j++){
    Settings.Pass[j]=t[(*i) + j];
  }

  EEPROM.put(0, Settings);

  strcpy_P( t, (const char *)sPASS );
  k=strlen(t);
  for (uint8_t j=0; j<5; j++){
    t[k+j] = Settings.Pass[j];
    if (j==4){t[k+j+1] = 0;}
  }
  
  return true;
}

bool GotRes (char* t){
  uint8_t i;
  strcpy_P( t, (const char *)tRELAY ); //
  i=strlen(t);
  t[i  ] =  char('0' + (Settings.RelayOnTime / 2));
  t[i+1] = '.';
  t[i+2] = char('0' + ((Settings.RelayOnTime % 2)*5) );
  t[i+3] = 0;
  return true;   
}

bool CharIsDigit(char s){
  return (s >= 0x30 && s <= 0x39);
}

bool Status(char* t){
    String mess; 
    
    mess = CELL;
    mess += (String)(100*gsm.CellLevel()/31) + F("%\n");
    asm("WDR");
    if (digitalRead(Relay1Pin)) {mess += RELAY1ON;}
    else {mess += RELAY1OFF;}
    asm("WDR");
    if (digitalRead(Relay2Pin)) {mess += RELAY2ON;}
    else {mess += RELAY2OFF;}
    asm("WDR");
    strcpy(t, mess.c_str()); 
    asm("WDR");
    mess="";

    return true;
}

bool ExtractPosition(char *t, uint8_t i, uint16_t *pos){
      if ( !CharIsDigit(t[i])  )        { return false;}
      *pos += ( uint8_t( t[i] - 0x30 ) ) * 100;
      
      if ( !CharIsDigit(t[(i)+1]) )      { return false;}
      *pos += ( uint8_t(t[(i)+1]  ) - 0x30 ) * 10;
      
      if ( !CharIsDigit(t[(i)+2]) )      { return false;}
      *pos += uint8_t(  t[(i)+2]  ) - 0x30;return true;
}

bool FindPosition(char *t, uint16_t *pos){
  word a = ( (*pos) - 1 ) * 6;
  uint8_t i;
  byte zip;

  if (*pos>Capacity) {return ErrorF(t);}
  
  t[0]= char( (*pos)/100      + '0' );
  t[1]= char( ((*pos)%100)/10 + '0' );
  t[2]= char( ((*pos)%100)%10 + '0' );
  t[3]= ':';
  t[4]= ' ';

  for(i=0; i<6; i++){
    zip = ReadMemory(a+i);
    
    if (i>0) {
           t[5+i*2-1] = char ('0' + ((zip & 0xF0)>>4));
           t[5+i*2]   = char ('0' + (zip & 0x0F));
    }else {if (zip==0) { i = 6; t[5]=0;} else {t[5] = char ('0' + (zip & 0x0F));}}
  }
  t[16]=0;
  return true;
}

bool DelNum(char *t, uint16_t *pos){
  //WRITE!!!!*******************************
  word a = ( (*pos) - 1 ) * 6;
  uint8_t i;
  if (*pos>Capacity) {return ErrorF(t);}
  
  ClearMemory(6, a);
  strcpy_P( t, (const char *)DELETED );
  i=strlen(t);
  //t[4]=' ';
  t[i  ]= char( (*pos)/100      + '0' );
  t[i+1]= char( ((*pos)%100)/10 + '0' );
  t[i+2]= char( ((*pos)%100)%10 + '0' );
  t[i+3] = 0;
  return true;
}

bool AddNum(char* t, uint16_t *pos){
  //WRITE!!!!*******************************
  word a = ( (*pos) - 1 ) * 6;
  uint8_t k;
 if (*pos>Capacity) {return ErrorF(t);}
 if (FindNum(t)){return true;}

  for(uint8_t i=0; i<6; i++){
    Wire.beginTransmission(AT24C32);
    Wire.write(highAddressByte(a+i));      //First Word Address
    Wire.write(lowAddressByte(a+i));      //Second Word Address
    
    Wire.write(PhoneComp[i]);      //Write
    delay(10);
    Wire.endTransmission();
  }

  strcpy_P( t, (const char *)ADDED );
  k=strlen(t);
  t[k  ]=' ';
  t[k+1]= char( (*pos)/100      + '0' );
  t[k+2]= char( ((*pos)%100)/10 + '0' );
  t[k+3]= char( ((*pos)%100)%10 + '0' );
  t[k+4] = 0;
  return true;  
}
