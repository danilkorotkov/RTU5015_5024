#include "protocol5015.h"
#include "defs.h"
#include <EEPROM.h>
#include "actions.h"

void CheckNum(char *p, char *t){
  uint8_t i = 0;
  if ( (Settings.AccessAlarm>>4) == 1 ) {Relay1();}
  else{
    while(strlen(p)>11){
      for (i=0; i<strlen(p); i++){
        p[i]=p[i+1];
      }
      p[strlen(p)] = 0;
    }
    
    i = strlen(p) - 1;
    if ( i>9 &&  ZipNum(p, &i) ) { if ( FindNum(t) ){ Relay1();}}
  }
}

bool CheckSMS(char* t){
  String Sequence = F("1234#RESET");
  uint8_t i = 0;

  if( FindSeq(&i, Sequence, t, i) && !ResetTimeout) {return Reset(t, false);}
  
  if( !(t[0] == Settings.Pass[0] && t[1] == Settings.Pass[1] && t[2] == Settings.Pass[2] && t[3] == Settings.Pass[3] && t[4] == '#') ){return ErrorF(t);}  
  
  i = 5;
  Sequence = F("FSET#");
  if( FindSeq(&i, Sequence, t, i) ) {return Reset(t, true);}
  
  //tel manipulation
  i = 5;
  Sequence = F("TEL");
  if( FindSeq(&i, Sequence, t, i) ) { return ConfTel(t, &i);}

  //pwd change 1234#PWD6666#PWD6666#
  Sequence = F("PWD");
  i = 5; 
  if( FindSeq(&i, Sequence, t, i) ) {return PwdChange(t, &i);}
  
  // доступ к реле 1234#AA# 1-all  1234#AU# 0=auth
  Sequence = F("AA#");
  i = 5; 
  if( FindSeq(&i, Sequence, t, i) ) {
    strcpy_P( t, (const char *)AA );
    Settings.AccessAlarm = (Settings.AccessAlarm & 0x0F) + 0x10;
    EEPROM.put(0, Settings);
    return true;
  }

  Sequence = F("AU#");
  i = 5; 
  if( FindSeq(&i, Sequence, t, i) ) {
    strcpy_P( t, (const char *)AU );
    Settings.AccessAlarm = Settings.AccessAlarm & 0x0F;
    EEPROM.put(0, Settings);
    return true;
  }

  //время реле
  Sequence = F("GOT");
  i = 5; 
  if( FindSeq(&i, Sequence, t, i) ){
    if  (t[i] == '?') {
      return GotRes(t);
    }
    else{
                                                                      //if (t[i+2] == '#' && t[i] >= 0x30 && t[i] <= 0x39 && t[i+1] >= 0x30 && t[i+1] <= 0x39) {
      if (t[i+2] == '#' && CharIsDigit(t[i]) && CharIsDigit(t[i+1])) {
        Settings.RelayOnTime = (uint8_t(t[i] - 0x30)) * 10 + (uint8_t(t[i+1] - 0x30));
        if (Settings.RelayOnTime<19){
          EEPROM.put(0, Settings);
          Timer1Init();
          return GotRes(t);
        }else {
          EEPROM.get(0, Settings);
          return ErrorF(t);
        }       
      }
    }
  }

  //статус
  Sequence = F("CSQ?");
  i = 5; 
  if( FindSeq(&i, Sequence, t, i) ){return Status(t);}
  
  //второе реле
  i = 4;
  Sequence = F("#ON#");
  if( FindSeq(&i, Sequence, t, i) ){return Relay2(true,  t);}
  i = 4;
  Sequence = F("#OFF#");
  if( FindSeq(&i, Sequence, t, i) ){return Relay2(false,  t);}

  //выходы 1234#KEY1NC#/1234#KEY2NC#
/*  i = 5; Sequence = F("KEY");
  if( FindSeq(&i, Sequence, t, i) ){
    if ( t[i] == '1' && t[i+1] == 'N' && t[i+3] == '#'){
      if ( t[i+2] == 'O') {Settings.DigitalInputType = Settings.DigitalInputType & 0x0F + 0x10; return AlarmInfo(t);}
      if ( t[i+2] == 'C') {Settings.DigitalInputType = Settings.DigitalInputType & 0x0F;        return AlarmInfo(t);}
    }
    if ( t[i] == '2' && t[i+1] == 'N' && t[i+3] == '#'){
      if ( t[i+2] == 'O') {Settings.DigitalInputType = Settings.DigitalInputType & 0xF0 + 0x01; return AlarmInfo(t);}
      if ( t[i+2] == 'C') {Settings.DigitalInputType = Settings.DigitalInputType & 0xF0;        return AlarmInfo(t);}
    }
  }*/

  return ErrorF(t);
}

bool ConfTel(char* t, uint8_t * i){
  uint16_t pos = 0;
  uint8_t j = 0;
  String Sequence = F("?");
 
//---------------search num by position or del------------------------------------------------------//   
    if ( t[(*i)+3] == '?' ||  t[(*i)+3] == '#') {
      if ( !ExtractPosition(t, *i, &pos) ) {return ErrorF(t);}           
      if ( t[(*i)+3] == '?' ) {return FindPosition (t, &pos);} else {return DelNum(t, &pos);}       
    }
//---------------search num by position or del end------------------------------------------------------//   
    
//-------------------------------------find by num------------------------------------------------------//   
    j = *i;  //Sequence = F("?");
    if( FindSeq(&j, Sequence, t, (*i)+11 ) ) {  
      //if ( j - (*i) > 11) { (*i) += j - (*i) - 11;}
      j -=2;                                                             
      if (ZipNum(t, &j)) {return FindNum(t);}
      else { return ErrorF(t);}
    }
//-------------------------------------find by num end ------------------------------------------------------// 
    
//-------------------------------------add num---------------------------------------------------------------//
    Sequence = F("#");
    j = *i;
    if( !FindSeq(&j, Sequence, t, (*i)+11 ) )                         { return ErrorF(t);}
    //if ( j - (*i) > 11) { (*i) += j - (*i) - 11;}
    *i = j;
    j -= 2; 
    if ( t[(*i)+3] == '#') {    
      if ( !ExtractPosition(t, *i, &pos) ) {return ErrorF(t);}
      if (ZipNum(t, &j)) {return AddNum(t, &pos);}
      else { return ErrorF(t);}
    } else { return ErrorF(t);}
}
