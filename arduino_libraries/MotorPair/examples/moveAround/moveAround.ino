
#include "MotorPair.h"

const int in1_pin = 5;
const int in2_pin = 6;
const int in3_pin = 9;
const int in4_pin = 10;

MotorPair motors(in1_pin, in2_pin, in3_pin, in4_pin);

void setup() {
	delay(100);
}

void loop() {
	
	for(int i=0; i<=250; i++) {
		motors.setSpeed(i,-i);
		delay(20);
	}
	for(int i=0; i<=250; i++) {
		motors.setSpeed(-i,i);
		delay(20);
	}
	motors.stopMotors();
	delay(2000);

	motors.setSpeed(100, 100);
	delay(2000);
	motors.setSpeed(100, 0);
	delay(2000);
	motors.setSpeed(0, 100);
	delay(2000);
	motors.stopMotors();
	delay(500);
	motors.setSpeed(-100, -100);
	delay(2000);
	motors.setSpeed(0, -100);
	delay(2000);
	motors.setSpeed(-100, 0);
	delay(2000);
	motors.stopMotors();
	delay(2000);
}
