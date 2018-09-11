
#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct Message_st {
  int left_speed1;
  int right_speed1;
  int left_speed2;
  int right_speed2;
  int left_speed3;
  int right_speed3;
  int checksum;
} Message;

unsigned int hashString(Message msg);

#endif
