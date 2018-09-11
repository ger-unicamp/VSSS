#ifndef ENCODERPAIR_H
#define ENCODERPAIR_H

#include <Arduino.h>

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
	volatile bool left_state;
	volatile bool right_state;
	volatile long int left_counter;
	volatile long int right_counter;


public:
	EncoderPair(const unsigned int left_pin, const unsigned int right_pin);
	void encoderCalibrate(const int null_percentage, const unsigned int iterations = ITERATION_NUMBER);
	void writeCalibrationToEEPROM(unsigned int base_adress);
	void readCalibrationFromEEPROM(unsigned int base_adress);
	void __read();
	void setCounters(const unsigned int left_counter, const unsigned int right_counter);
	String toString();
	String counterString();
};

#endif
