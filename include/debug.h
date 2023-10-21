#include "Arduino.h"

#define ENABLE_PRINT_DEBUG 0

void debug_init();

void print(String value, int8_t number = 0);

void print(int value);

void print(long value);

void print(unsigned int value);