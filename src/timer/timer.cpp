#include "timer.h"

esp_timer_handle_t timer = NULL;
uint32_t microseconds = 1000000;

void init_timer(int seconds, esp_timer_cb_t callback)
{
  if (timer != NULL)
  {
    esp_timer_stop(timer);
    esp_timer_delete(timer);
    timer = NULL;
  }

  uint64_t period = seconds * microseconds;

  esp_timer_create_args_t timer_args = {
      .callback = callback,
      .arg = NULL,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "timer_menu"};
  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, period);
}

void stop_timer()
{
  if (timer != NULL)
  {
    esp_timer_stop(timer);
    esp_timer_delete(timer);
    timer = NULL;
  }
}