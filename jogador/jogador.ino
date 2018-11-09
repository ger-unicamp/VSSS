#include "jogador.h"

#define DEBUG_MODE
#undef DEBUG_MODE
#ifdef DEBUG_MODE
#define debug_print(x) (Serial.println((x)))
#else
#define debug_print(x)
#endif

#define LED_MESSAGE 13

MotorPair motors(PH_IN1, PH_IN2, PH_IN3, PH_IN4);

RF24 radio(RF_CE, RF_CSN);
const uint64_t pipe = 0xA2E8F0F0E1LL;

Message message;

void setup() {

	// Radio init
	radio.begin();
	radio.openReadingPipe(1, pipe);
	radio.startListening();


#ifdef DEBUG_MODE
	Serial.begin(57600);
	delay(500);
	debug_print("FINISHING SETUP");
#endif

	pinMode(LED_MESSAGE, OUTPUT);
}

int rSpeed = 0;
int lSpeed = 0;

void loop() {

	if (radio.available()) {
		bool done = false;
		debug_print("Radio available");
		while (!done) {
			done = radio.read(&message, sizeof(message));
			if(message.checksum != hashMessage(message)) {
				debug_print("Checksum invalid");
				continue;
			}
			if (!done) {
				debug_print("Lost package");
			}

			if (PLAYER == 0) {
				lSpeed = message.left_speed0;
				rSpeed = message.right_speed0;
			}
			else if (PLAYER == 1) {
				lSpeed = message.left_speed1;
				rSpeed = message.right_speed1;
			}
			else if (PLAYER == 2) {
				lSpeed = message.left_speed2;
				rSpeed = message.right_speed2;
			}

			debug_print(messageToString(message));
			#ifdef DEBUG_MODE
				Serial.flush();
			#endif
			digitalWrite(LED_MESSAGE, !digitalRead(LED_MESSAGE));
		}
	}
	else {
		//debug_print("Radio not available");
	}
	//debug_print(lSpeed);
	//debug_print(rSpeed);

	if (lSpeed > 255)
		lSpeed = 255;
	else if (lSpeed < -255)
		lSpeed = -255;
	if (rSpeed > 255)
		rSpeed = 255;
	else if (rSpeed < -255)
		rSpeed = -255;

	motors.setSpeed(lSpeed, rSpeed);
}
