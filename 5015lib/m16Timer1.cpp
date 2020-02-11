#include "m16Timer1.h"

// ************************** TIMER 1 ***************************
void (*isr1)();
volatile uint16_t isr1_counter;

void timer1_ISR(void (*isr)()) {
	isr1 = *isr;
}

void timer1_setPeriod(uint32_t time) {
	TCCR1A = 0;
	TCCR1B = 0;

/*	if (time > 5 && time < 4096) {
		time = 65584 - (time << 4);                   //от 5 до 4096 мкс 
		TCCR1B = (1 << CS10);                         //001 - тактовый генератор CLK
	
	} else if (time > 4095 && time < 32768) {
		time = 65542 - (time << 1);
		TCCR1B = (1 << CS11);                         //010 - CLK/8
	
	} else if (time > 32767 && time < 262144) {
		time = 65536 - (time >> 2);
		TCCR1B = ((1 << CS11) | (1 << CS10));         //011 - CLK/64
	
	} else if (time > 262143 && time < 1048576) {
		time = 65536 - (time >> 4);
		TCCR1B = (1 << CS12);                         //100 - CLK/256
	
	} else if (time > 1048575 && time < 4194304) {
		time = 65536 - (time >> 6);
		TCCR1B = ((1 << CS12) | (1 << CS10));         //101 - CLK/1024
	
	} else TCCR1B = 1;*/

  isr1_counter = 65536 - time/139;                      // переполнение 9 102 222 мкс, 138,89 мкс на тик
  TCCR1B = ((1 << CS12) | (1 << CS10));         //101 - CLK/1024
  TIMSK &= ~(1 << TOIE1);  // запретить прерывания по таймеру1
	
	TCNT1 = 0;  				// выставляем начальное значение TCNT1
}

void timer1_start(void) {
  TCNT1 = isr1_counter;           // выставляем начальное значение TCNT1
  if (isr1_counter != 65536){     //got00
    TIFR |= (1 << TOV1);          //флаг переполнения чистим
    TIMSK |= (1 << TOIE1);        // разрешаем прерывание по переполнению таймера  
  }
}

void timer1_stop(void) {
	TIMSK &= ~(1 << TOIE1);	// запретить прерывания по таймеру1
}


ISR(TIMER1_OVF_vect) {
  timer1_stop();
	TCNT1 = isr1_counter;
	(*isr1)();
}
