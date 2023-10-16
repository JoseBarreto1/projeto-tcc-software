#include "display.h"
#include "camera.h"
#include "debug.h"

static inline camera_config_t app_camera_config()
{
	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;

	config.frame_size = FRAMESIZE_UXGA;
	config.jpeg_quality = 10;
	config.fb_count = 1;

	return config;
}

void init_sensor_camera()
{
	sensor_t *s = esp_camera_sensor_get();
	s->set_framesize(s, FRAMESIZE_QQVGA2); // define resolução 128x160
	s->set_vflip(s, 1);
}

camera_config_t config = app_camera_config();

void camera_init()
{
	static esp_err_t cam_err = esp_camera_init(&config);
	if (cam_err != ESP_OK)
	{
		print("A inicialização da câmera falhou com o seguinte erro: 0x%x", cam_err);
		return;
	}
}

void camera_live_stream(camera_fb_t *frame_buffer)
{
	if (frame_buffer != NULL) 
	{
		esp_camera_fb_return(frame_buffer);
		frame_buffer = NULL;
	}
	frame_buffer = esp_camera_fb_get();
	display_live_stream((const uint8_t *)frame_buffer->buf, frame_buffer->len);
	delay(100);
}