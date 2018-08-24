
#include "MillisInterrupt.h"

volatile unsigned long x = 0;

// Interrupt is called once a millisecond
void ping() {
	x++;
	if (x % 1000 == 0)
		digitalWrite(13, !digitalRead(13));
}

void setup() {
	pinMode(13, OUTPUT);
	MillisInterrupt::set(ping);
	MillisInterrupt::start();
	delay(1000);
}

void loop() {

}
