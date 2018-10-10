/*
 *	Library for VSSS specified data transmission.
 *	Rev 06/10/2018
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>

typedef struct Message_st {
  int left_speed0;
  int right_speed0;
  int left_speed1;
  int right_speed1;
  int left_speed2;
  int right_speed2;
  int checksum;
} Message;

unsigned int hashMessage(Message msg);
String messageToString(Message msg);

#endif
