#include "SPI.h"
#include "TFT_eSPI.h"
#include "TFT_eFEX.h"

void display_init();

void init_SPIFFS();

void display_password(int current_position, int password_temp[3]);

void display_write_string(String text);

void display_write_char(char * text, int16_t x, int16_t y);

void display_success();

void display_error();

void display_menu();

void display_initial_count();

void display_live_stream(const uint8_t * buffer, size_t length);