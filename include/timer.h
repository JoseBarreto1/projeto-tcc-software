#include "esp_timer.h"

#define TIME_DEFAULT 40

void init_timer(int seconds, esp_timer_cb_t callback);