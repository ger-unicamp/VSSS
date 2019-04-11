/**
 * @file MillisInterrupt.cpp
 * @author Grupo de Estudos em Robótica - GER
 * @brief Call a function in each 1ms
 * @date 2019-04-11
 * @info Code based on Adafruit's code at
 * https://learn.adafruit.com/pages/4570/elements/1081214/download
 * 
 */

#include "MillisInterrupt.h"

static void (*function) (void) = NULL;

void MillisInterrupt::set(void (*f) (void)) {
	function = f;
}

/**
 * @brief Timer0 is already used for millis() - we'll just interrupt somewhere
 * in the middle and call the "Compare A" function below
 */
void MillisInterrupt::start() {
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
}

/**
 * @brief Interrupt is called once a millisecond
 */
SIGNAL(TIMER0_COMPA_vect) {
	if (function != NULL)
		(*function)();
}
