#include <message.h>

unsigned int hashString(Message msg) {
  unsigned char *str = (unsigned char *) &msg;

  unsigned int hash = 5381;
  for(int i = 0; i < 12; i++)
    hash = ((hash << 5) + hash) + *(str + i); // hash * 33 + c

  return hash;
}
