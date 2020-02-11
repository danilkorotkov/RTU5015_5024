#ifndef protocol5015_h
#define protocol5015_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "defs.h"
#include "MC35i.h"

extern                SetsStruct Settings;
extern                MC35i gsm;
extern bool           ResetTimeout; 

void CheckNum         (char *, char *);
bool CheckSMS         (char*);
bool ConfTel          (char*, uint8_t *);

#endif
