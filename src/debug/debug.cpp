#include "debug.h"

void print(String value, int8_t number)
{
	if (ENABLE_PRINT_DEBUG == 1)
	{
		if (number != 0)
		{
			const char *text_c_str = value.c_str();
			Serial.printf(const_cast<char *>(text_c_str), number);
		}
		else
		{
			Serial.println(value);
		}
	}
	return;
}

void print(int value)
{
	if (ENABLE_PRINT_DEBUG == 1)
	{
		Serial.println(value);
	}
	return;
}

void print(long value)
{
	if (ENABLE_PRINT_DEBUG == 1)
	{
		Serial.println(value);
	}
	return;
}

void print(unsigned int value)
{
	if (ENABLE_PRINT_DEBUG == 1)
	{
		Serial.println(value);
	}
	return;
}