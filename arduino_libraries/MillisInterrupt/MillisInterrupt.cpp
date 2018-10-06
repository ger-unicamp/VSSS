/*
 *	MillisInterrupt.h - Call a function in each 1ms
 *
 *	From Unicamp's Grupo de Estudos em Robótica - GER
 *
 *	Code based on Adafruit's code at
 *	https://learn.adafruit.com/pages/4570/elements/1081214/download
 *
 *	Rev 06/10/2018
 */

#include "MillisInterrupt.h"

static void (*function) (void) = NULL;

void MillisInterrupt::set(void (*f) (void)) {
	function = f;
}

void MillisInterrupt::start() {
	// Timer0 is already used for millis() - we'll just interrupt somewhere
  	// in the middle and call the "Compare A" function below
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
}

// Interrupt is called once a millisecond
SIGNAL(TIMER0_COMPA_vect) {
	if (function != NULL)
		(*function)();
}
