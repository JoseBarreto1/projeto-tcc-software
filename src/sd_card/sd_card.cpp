#include "FS.h"		// SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32
#include "debug.h"

// Exemplo de uso:

// void setup()
// { ...
// 	init_card()
// ...}

// void gravar_foto() 
// { ...
// 		camera_fb_t * frame_buffer = esp_camera_fb_get();
// 		write_card(*user_number, frame_buffer->buf, frame_buffer->len);
// 		esp_camera_fb_return(frame_buffer);
// 		frame_buffer = NULL;
// ...}


void init_card()
{
	if (!SD_MMC.begin("/sdcard", true))
	{
		print("Falha ao inicializar o cartão SD");
		return;
	}

	uint8_t cardType = SD_MMC.cardType();
	if (cardType == CARD_NONE)
	{
		print("No SD Card attached");
		return;
	}
	print("CardType: %d", cardType);
}

void write_card(int file_number, const uint8_t *src_buf, size_t src_len)
{
	fs::FS &fs = SD_MMC; 
	char *name_file = (char *)malloc(23 + sizeof(size_t));
	sprintf(name_file, "/face_detect_user_%d.jpg", file_number);

	File file = fs.open(name_file, FILE_WRITE);
	print(name_file);
	free(name_file);

	file.write(src_buf, src_len);
	file.close();
}