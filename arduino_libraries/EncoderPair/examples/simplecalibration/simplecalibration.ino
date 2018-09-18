#include <EncoderPair.h>
#include <MotorPair.h>
#include <MillisInterrupt.h>

const int in1_pin = 5;
const int in2_pin = 6;
const int in3_pin = 9;
const int in4_pin = 10;

MotorPair motores(in1_pin, in2_pin, in3_pin, in4_pin);

const int left_encoder_pin = A5;
const int right_encoder_pin = A4;

EncoderPair encoders(left_encoder_pin, right_encoder_pin);

void leitura() {
	encoders.__read();
}

String impressao;

void setup() {
	Serial.begin(9600);
	delay(5000);
	Serial.println("Inicializando\n");

	motores.setSpeed(100, -100);
	encoders.encoderCalibrate(10);
	motores.stopMotors();
	delay(500);
	motores.setSpeed(-100, 100);
	encoders.encoderCalibrate(10);
	motores.stopMotors();
	delay(500);

	Serial.println("Fim da calibracao!\n");
	impressao = encoders.toString();
	Serial.println(impressao);
	delay(1000);

	MillisInterrupt::set(leitura);
	MillisInterrupt::start();
}

void loop() {
	impressao = encoders.counterString();
	Serial.println(impressao);
	delay(1000);
}
