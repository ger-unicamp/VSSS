#define PLAYER 0

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "message.h"
#include "MotorPair.h"


/**	H-BRIDGE PINS **/

// LEFT MOTOR
#define PH_IN1 5
#define PH_IN2 6
// RIGHT MOTOR
#define PH_IN3 9
#define PH_IN4 10

/** RF24 PINS **/

#define RF_CSN 8
#define RF_CE 7

/** ENCODER PINS **/

#define ENC_L A5
#define ENC_R A4
