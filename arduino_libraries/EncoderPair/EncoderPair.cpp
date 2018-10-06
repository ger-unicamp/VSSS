/*
 *	Library for analog reflecting sensor as an enconder.
 *	Rev 06/10/2018
 */
#include <EncoderPair.h>

EncoderPair::EncoderPair(const unsigned int left_pin, const unsigned int right_pin) {
	this->left_pin = left_pin;
	this->right_pin = right_pin;
	this->high_threshold_left = 512;
	this->high_threshold_right = 512;
	this->low_threshold_left = 512;
	this->low_threshold_right = 512;
	this->left_state = false;
	this->right_state = false;
	this->left_counter = 0;
	this->right_counter = 0;
	this->is_calibrated = false;
}

void EncoderPair::encoderCalibrate(const int null_percentage, const unsigned int iterations) {
	int max_left_value = 0;
	int min_left_value = 1023;
	int max_right_value = 0;
	int min_right_value = 1023;

	if (this->is_calibrated)
	{
		max_left_value = ((100.0 / null_percentage + 1) * high_threshold_left + (1 - 100.0 / null_percentage) * low_threshold_left) / 2;
		min_left_value = ((1 - 100.0 / null_percentage) * high_threshold_left + (100.0 / null_percentage + 1) * low_threshold_left) / 2;

		max_right_value = ((100.0 / null_percentage + 1) * high_threshold_right + (1 - 100.0 / null_percentage) * low_threshold_right) / 2;
		min_right_value = ((1 - 100.0 / null_percentage) * high_threshold_right + (100.0 / null_percentage + 1) * low_threshold_right) / 2;
	}

	for (unsigned long i = 0; i < iterations; i++) {
		max_left_value = max(max_left_value, analogRead(this->left_pin));
		min_left_value = min(min_left_value, analogRead(this->left_pin));

		max_right_value = max(max_right_value, analogRead(this->right_pin));
		min_right_value = min(min_right_value, analogRead(this->right_pin));
	}

	float null_interval_left = null_percentage * (max_left_value - min_left_value) / 100.0;
	high_threshold_left = (max_left_value + min_left_value + null_interval_left) / 2;
	low_threshold_left = (max_left_value + min_left_value - null_interval_left) / 2;

	float null_interval_right = null_percentage * (max_right_value - min_right_value) / 100.0;
	high_threshold_right = (max_right_value + min_right_value + null_interval_right) / 2;
	low_threshold_right = (max_right_value + min_right_value - null_interval_right) / 2;

	this->is_calibrated = true;
}

void EncoderPair::writeCalibrationToEEPROM(unsigned int base_adress) {
	EEPROM.write(base_adress+0, (byte) (this->high_threshold_left >> 8));
	EEPROM.write(base_adress+1, (byte) (this->high_threshold_left & 0xff));

	EEPROM.write(base_adress+2, (byte) (this->low_threshold_left >> 8));
	EEPROM.write(base_adress+3, (byte) (this->low_threshold_left & 0xff));

	EEPROM.write(base_adress+4, (byte) (this->high_threshold_right >> 8));
	EEPROM.write(base_adress+5, (byte) (this->high_threshold_right & 0xff));

	EEPROM.write(base_adress+6, (byte) (this->low_threshold_right >> 8));
	EEPROM.write(base_adress+7, (byte) (this->low_threshold_right & 0xff));
}

void EncoderPair::readCalibrationFromEEPROM(unsigned int base_adress) {
	this->high_threshold_left = (((int) EEPROM.read(base_adress+0)) << 8) + EEPROM.read(base_adress+1);
	this->low_threshold_left = (((int) EEPROM.read(base_adress+2)) << 8) + EEPROM.read(base_adress+3);

	this->high_threshold_right = (((int) EEPROM.read(base_adress+4)) << 8) + EEPROM.read(base_adress+5);
	this->low_threshold_right = (((int) EEPROM.read(base_adress+6)) << 8) + EEPROM.read(base_adress+7);
}

void EncoderPair::__read() {
	int left_read = analogRead(this->left_pin);
	if (left_read > this->high_threshold_left && this->left_state == false) {
		this->left_state = true;
		this->left_counter = this->left_counter + 1;
	}
	else if (left_read < this->low_threshold_left && this->left_state == true) {
		this->left_state = false;
		this->left_counter = this->left_counter + 1;
	}

	int right_read = analogRead(this->right_pin);
	if (right_read > this->high_threshold_right && this->right_state == false) {
		this->right_state = true;
		this->right_counter = this->right_counter + 1;
	}
	else if (right_read < this->low_threshold_right && this->right_state == true) {
		this->right_state = false;
		this->right_counter = this->right_counter + 1;
	}
}

void EncoderPair::setCounters(const unsigned int left_counter, const unsigned int right_counter) {
	this->left_counter = left_counter;
	this->right_counter = right_counter;
}

long int getLeftCounter() {
	return this->left_counter;
}

long int getRightCounter() {
	return this->right_counter;
}

String EncoderPair::toString() {
	String s = "PINS: ";
	s += String("")  + this->left_pin + " " + this->right_pin + "\n";
	s += "LEFT VALUES: ";
	s += String("") + this->high_threshold_left + " " + this->low_threshold_left + "\n";
	s += "RIGHT VALUES: ";
	s += String("") + this->high_threshold_right + " " + this->low_threshold_right + "\n";

	return s;
}

String EncoderPair::counterString() {
	String s = "ANALOG VALUES: ";
	s += String("") + analogRead(this->left_pin) + " " + analogRead(this->right_pin) + "\n";
	s += "COUNTER VALUES: ";
	s += String("") + this->left_counter + " " + this->right_counter + "\n";

	return s;
}
