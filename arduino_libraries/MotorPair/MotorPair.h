/*
 *	MotorPair.h - Control a robot's motors
 *
 *	From Unicamp's Grupo de Estudos em Robótica - GER
 *
 */

#ifndef MOTORPAIR_H
#define MOTORPAIR_H

#include <Arduino.h>

class MotorPair {
private:
	unsigned int in1;
	unsigned int in2;
	unsigned int in3;
	unsigned int in4;
	int ASpeed;
	int BSpeed;
	void init();
public:

	MotorPair(const unsigned int in1, const unsigned int in2, const unsigned int in3, const unsigned int in4);
	void setSpeed(const int motorA, const int motorB, float alpha=0);
	void stopMotors();
};

#endif
