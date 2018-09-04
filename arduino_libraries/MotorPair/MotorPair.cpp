/*
 *	MotorPair.h - Control a robot's motors
 *
 *	From Unicamp's Grupo de Estudos em Robótica - GER
 *
 */

#include <Arduino.h>
#include <MotorPair.h>

MotorPair::MotorPair(	const unsigned int in1, const unsigned int in2,\
						const unsigned int in3, const unsigned int in4) {
	this->in1 = in1;
	this->in2 = in2;
	this->in3 = in3;
	this->in4 = in4;
	this->init();
}

void MotorPair::init() {
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
}

void MotorPair::setSpeed(const int motorA, const int motorB) {
	if (motorA > 0) {
		analogWrite(in1, motorA);
		digitalWrite(in2, LOW);
	} else if (motorA == 0) {
		digitalWrite(in1, LOW);
		digitalWrite(in2, LOW);
	} else {
		digitalWrite(in1, LOW);
		analogWrite(in2, -motorA);
	}

	if (motorB > 0) {
		analogWrite(in3, motorB);
		digitalWrite(in4, LOW);
	} else if (motorB == 0) {
		digitalWrite(in3, LOW);
		digitalWrite(in4, LOW);
	} else{
		digitalWrite(in3, LOW);
		analogWrite(in4, -motorB);
	}
}

void MotorPair::stopMotors() {
    this->setSpeed(0, 0);
}
