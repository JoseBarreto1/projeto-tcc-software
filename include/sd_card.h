#include "SD.h"

void init_card();

void write_card(int file_number, const uint8_t *src_buf, size_t src_len);