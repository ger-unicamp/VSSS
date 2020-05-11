/**
 * @brief Code for write on Arduino Serial
 *        Based on http://www.cplusplus.com/forum/beginner/67479/
 * 
 */

#ifndef _ARDUINO_SERIAL_H_
#define _ARDUINO_SERIAL_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <stdexcept>

#include <random>
#include <chrono>

#define DEBUG_PRINT_SERIAL
//#undef DEBUG_PRINT_SERIAL

class SerialSender
{
  private:
    int serialDescriptor;
    char buffer[1000];

  public:
    SerialSender(const char *serialPath, unsigned int baud);
    ~SerialSender();
    void serialclose();
    void send(int vel_1l, int vel_1r, int vel_2l, int vel_2r, int vel_3l, int vel_3r);
};

#endif
