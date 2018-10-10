/*
 *	Library for VSSS specified data transmission.
 *	Rev 06/10/2018
 */

#include <message.h>

unsigned int hashMessage(Message msg) {
  unsigned char *str = (unsigned char *) &msg;

  unsigned int hash = 5381;
  for(int i = 0; i < (sizeof(Message)-sizeof(int)); i++)
    hash = ((hash << 5) + hash) + *(str + i); // hash * 33 + c

  return hash;
}

String messageToString(Message msg) {
    String s;

    s  = String("") + msg.left_speed0 + " " + msg.right_speed0 + "\n";
    s += String("") + msg.left_speed1 + " " + msg.right_speed1 + "\n";
    s += String("") + msg.left_speed2 + " " + msg.right_speed2 + "\n";
    s += String("") + msg.checksum + "\n";

    return s;
}
