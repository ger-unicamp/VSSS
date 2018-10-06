/*
 *	Library for analog reflecting sensor as an enconder.
 *	Rev 06/10/2018
 */

#ifndef ENCODERPAIR_H
#define ENCODERPAIR_H

#include <Arduino.h>
#include <EEPROM.h>

#define ITERATION_NUMBER 10000

class EncoderPair {
private:
	unsigned int left_pin;
	unsigned int right_pin;
	unsigned int high_threshold_left;
	unsigned int high_threshold_right;
	unsigned int low_threshold_left;
	unsigned int low_threshold_right;
	bool is_calibrated;
	bool left_state;
	bool right_state;
	long int left_counter;
	long int right_counter;

public:
	EncoderPair(const unsigned int left_pin, const unsigned int right_pin);
	void encoderCalibrate(const int null_percentage, const unsigned int iterations = ITERATION_NUMBER);
	void writeCalibrationToEEPROM(unsigned int base_adress);
	void readCalibrationFromEEPROM(unsigned int base_adress);
	void __read();
	void setCounters(const unsigned int left_counter = 0, const unsigned int right_counter = 0);
	long int getLeftCounter();
	long int getRightCounter();
	String toString();
	String counterString();
};

#endif
