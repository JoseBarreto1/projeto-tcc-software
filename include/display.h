#include "SPI.h"
#include "TFT_eSPI.h"
#include "TFT_eFEX.h"

#define TEXT_SIZE_DEFAULT 1
#define TEXT_SIZE_MEDIUM 2

void display_init();

void init_SPIFFS();

void display_password(int current_position, int password_temp[3]);

void display_write_string(String text, uint8_t text_size = TEXT_SIZE_DEFAULT, uint32_t color = TFT_BLACK, int16_t text_color = TFT_WHITE);

void display_write_char(char * text, int16_t x, int16_t y, uint32_t color, int16_t text_color, uint8_t text_size);

void display_success();

void display_error();

void display_menu();

void display_initial_count();

void display_live_stream(const uint8_t * buffer, size_t length);