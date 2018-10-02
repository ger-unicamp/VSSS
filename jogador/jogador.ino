#include "jogador.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define debug_print(x) \
	if (DEBUG_TEST) Serial.println(x)


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
	Serial.begin(115200);
	delay(500);
	debug_print("FINISHING SETUP");
#endif

}

void loop() {
	int rSpeed;
	int lSpeed;

	if (radio.available()) {
		bool done = false;
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
		}
	}
	else {
		debug_print("Radio not available");
	}

	motors.setSpeed(lSpeed, rSpeed);
}
