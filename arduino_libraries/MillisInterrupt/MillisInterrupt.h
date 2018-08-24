/*
 *	MillisInterrupt.h - Call a function in each 1ms
 *
 *	From Unicamp's Grupo de Estudos em Robótica - GER
 *
 *	Code based on Adafruit's code at
 *	https://learn.adafruit.com/pages/4570/elements/1081214/download
 */

#ifndef MILLISINTERRUPT_H
#define MILLISINTERRUPT_H

#include <Arduino.h>

namespace MillisInterrupt {
	void set(void (*function)(void));
	void start();
}

#endif
