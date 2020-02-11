#ifndef m16Timer1_h
#define m16Timer1_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

extern void           (*isr1)();

void timer1_ISR       (void (*isr)());			// подключить прерывание
void timer1_setPeriod (uint32_t time);	// установить период (мкс)
void timer1_start     (void);				// запустить
void timer1_stop      (void);					// остановить

#endif
