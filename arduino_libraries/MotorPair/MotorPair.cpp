/**
 * @file MotorPair.cpp
 * @author Grupo de Estudos em Robótica - GER
 * @brief The purpose of the code is to control a robot's motors.
 * @date 2019-04-11
 */

#include <Arduino.h>
#include <MotorPair.h>

/**
 * @brief Declaration of variables in1, in2, in3 and in4. They will serve 
 * to control de PWM's value of which motor. 
 */
MotorPair::MotorPair(	const unsigned int in1, const unsigned int in2,\
						const unsigned int in3, const unsigned int in4) {
	this->in1 = in1;
	this->in2 = in2;
	this->in3 = in3;
	this->in4 = in4;
	this->init();
	this->ASpeed = 0;
	this->BSpeed = 0;

}
/**
 * @brief Set the variables in1, in2, in3 and in4 as output.
 */
void MotorPair::init() {
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
}

/**
 * @brief Set the speed of the motors
 * @param motorA The first motor
 * @param motorB The second motor
 * @param alpha 
 */
void MotorPair::setSpeed(const int motorA, const int motorB, float alpha) {

	this->ASpeed=alpha*ASpeed+(1-alpha)*motorA;
	this->BSpeed=alpha*BSpeed+(1-alpha)*motorB;

	/**
	 * Positive values of ASpeed makes the motor spin to the right 
	 * and negative values makes him spin to the left. 
	 * When ASpeed iguals zero, the motor is stopped.
	 */
	if (ASpeed > 0) {
		analogWrite(in1, ASpeed);
		digitalWrite(in2, LOW);
	} else if (ASpeed == 0) {
		digitalWrite(in1, LOW);
		digitalWrite(in2, LOW);
	} else {
		digitalWrite(in1, LOW);
		analogWrite(in2, -ASpeed);
	}

	/**
	 * Follow the same logic of ASpeed.
	 */
	if (BSpeed > 0) {
		analogWrite(in3, BSpeed);
		digitalWrite(in4, LOW);
	} else if (BSpeed == 0) {
		digitalWrite(in3, LOW);
		digitalWrite(in4, LOW);
	} else{
		digitalWrite(in3, LOW);
		analogWrite(in4, -BSpeed);
	}
}

/**
 * @brief The motors are stopped.
 */
void MotorPair::stopMotors() {
    this->setSpeed(0, 0);
}
