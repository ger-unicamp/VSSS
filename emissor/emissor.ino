/*
Este programa envia duas variáveis inteiras, via rádio usando um módulo nRF24L01
No nosso Arduino Uno emissor sao (9,10).
As portas para comunicacao SPI sao definidas por padrao no Uno e no Nano como: MOSI 11, MISO 12, SCK 13.
*/

#define DEBUG_MODE
#undef DEBUG_MODE
#ifdef DEBUG_MODE
#define debug_print(x) (Serial.println((x)))
#else
#define debug_print(x)
#endif

#define BAUD 57600

#define CE_PIN 9
#define CSN_PIN 10

#define LED_MESSAGE 2

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "message.h"

RF24 radio(CE_PIN,CSN_PIN);

const uint64_t pipe = 0xA2E8F0F0E1LL;

Message message;

void setup(void)
{
    Serial.begin(BAUD);
    radio.begin();
    radio.openWritingPipe(pipe);
    pinMode(LED_MESSAGE, OUTPUT);
    delay(500);

    message = {0, 0, 0, 0, 0, 0, 0};
}

int i;
bool hasOpened = false;
String instruction[6] = {"","","","","",""};

void loop(void)
{
    while(Serial.available()){
        char c = Serial.read();
        if (c == '[') {
            i = 0;
            instruction[0] = "";
            hasOpened = true;
        }
        else if (c == ']' && hasOpened) {

            hasOpened = false;
            message.left_speed0 = instruction[0].toInt();
            message.right_speed0 = instruction[1].toInt();
            message.left_speed1 = instruction[2].toInt();
            message.right_speed1 = instruction[3].toInt();
            message.left_speed2 = instruction[4].toInt();
            message.right_speed2 = instruction[5].toInt();
            message.checksum = hashMessage(message);

            radio.write(&message, sizeof(message));

            digitalWrite(LED_MESSAGE, !digitalRead(LED_MESSAGE));
            debug_print(messageToString(message));

        }
        else if (hasOpened) {
            if (c == ',') {
                i++;
                instruction[i] = "";
            }
            else if (i < 6 && (c == '-' || isdigit(c))) {
                instruction[i] += c;
            }
        }
    }
}
