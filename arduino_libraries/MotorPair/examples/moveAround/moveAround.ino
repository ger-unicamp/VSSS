
#include "MotorPair.h"

const int in1_pin = 5;
const int in2_pin = 6;
const int in3_pin = 9;
const int in4_pin = 10;

MotorPair motores(in1_pin, in2_pin, in3_pin, in4_pin);

void setup() {
	delay(100);
}

void loop() {
	for(int i=0; i<=250; i+=50) {
		motores.setSpeed(i,-i);
		delay(1000);
	}
	for(int i=0; i<=250; i+=50) {
		motores.setSpeed(-i,i);
		delay(1000);
	}
	motores.stopMotors();
	delay(3000);
}
