#ifndef actions_h
#define actions_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "defs.h"
#include "MC35i.h"


extern                SetsStruct Settings;
extern uint8_t        PhoneComp[6];
extern                MC35i gsm;


bool ZipNum           (char* , uint8_t *);
bool FindNum          (char* );

bool Relay2           (bool, char*);
void Relay1           (void);
void Timer1Init       (void);
void INT_TIMER1       (void);

bool ErrorF           (char* );

byte ReadMemory       (word);
void ClearMemory      (uint16_t, uint16_t);
byte highAddressByte  (word);
byte lowAddressByte   (word);

void INT_TIMER1       (void);

bool FindSeq          (uint8_t *, String &, char *, uint8_t);
bool Reset            (char *, bool);
bool PwdChange        (char *, uint8_t *);
bool GotRes           (char *);
bool CharIsDigit      (char);
bool Status           (char *);
bool ExtractPosition  (char *, uint8_t, uint16_t *);
bool FindPosition     (char *, uint16_t *);
bool DelNum           (char *, uint16_t *);
bool AddNum           (char *, uint16_t *);



#endif
