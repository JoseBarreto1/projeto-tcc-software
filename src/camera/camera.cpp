#include "camera.h"

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
	config.fb_count = 2;
	return config;
}

void init_sensor_camera()
{
	sensor_t *s = esp_camera_sensor_get();
	s->set_framesize(s, FRAMESIZE_QQVGA2); // 128x160
	s->set_vflip(s, 1);
}

camera_config_t config = app_camera_config();

void camera_init()
{
	static esp_err_t cam_err = esp_camera_init(&config);
	if (cam_err != ESP_OK)
	{
		//printf("A inicialização da câmera falhou com o seguinte erro: 0x%x", cam_err);
		return;
	}
}
