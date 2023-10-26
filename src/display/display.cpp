#include "display.h"
#include "debug.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eFEX fex = TFT_eFEX(&tft);

void display_init()
{
	tft.begin();
	tft.setRotation(2); // 0 & 2 Portrait. 1 & 3 landscape
	display_write_string("  Bem   \n \n   vindo ", TEXT_SIZE_MEDIUM, TFT_DARKGREEN);
	delay(250);
}

void init_SPIFFS()
{
	if (!SPIFFS.begin())
	{
		print("Falha na inicialização do SPIFFS!");
		SPIFFS.begin(true);
	}

	fex.listSPIFFS(); // Lista os arquivos do SPIFFS
}

void display_password(int current_position, int password_temp[3])
{
	char *display_text = (char *)malloc(95 + sizeof(size_t));
	sprintf(display_text, "Digite a senha: \n \n  posicao atual: [%d]  \n \n   -  segredo:  - \n \n  -  [ %d/ %d / %d ]  -", current_position, password_temp[0], password_temp[1], password_temp[2]);
	print(display_text);
	display_write_char(display_text, 20, 45, TFT_BLACK, TFT_WHITE, TEXT_SIZE_DEFAULT);
	delay(200);

	free(display_text);
}

void display_write_string(String text, uint8_t text_size, uint32_t color, int16_t text_color)
{
	const char *text_c_str = text.c_str();
	display_write_char(const_cast<char *>(text_c_str), 25, 55, color, text_color, text_size);
}

void display_write_char(char *text, int16_t x, int16_t y, uint32_t color, int16_t text_color, uint8_t text_size)
{
	tft.fillScreen(color);
	tft.setCursor(x, y);
	tft.setTextColor(text_color);
	tft.setTextSize(text_size);
	tft.println(text);
	delay(200);
}

void display_success()
{
	fex.drawJpgFile(SPIFFS, "/_success.jpeg", 0, 0);
	delay(300);
	tft.fillScreen(TFT_BLACK);
	delay(50);
}

void display_error()
{
	fex.drawJpgFile(SPIFFS, "/_failure.jpeg", 0, 0);
	delay(600);
	tft.fillScreen(TFT_BLACK);
	delay(50);
}

void display_menu()
{
	fex.drawJpgFile(SPIFFS, "/_initial.jpeg", 0, 0);
	delay(200); // Debounce
}

void display_initial_count()
{
	fex.drawJpgFile(SPIFFS, "/_count3.jpg", 0, 0);
	delay(400);
	fex.drawJpgFile(SPIFFS, "/_count2.jpg", 0, 0);
	delay(400);
	fex.drawJpgFile(SPIFFS, "/_count1.jpg", 0, 0);
	delay(400);
	tft.fillScreen(TFT_BLACK);
	delay(100);
}

void display_live_stream(const uint8_t *buffer, size_t length)
{
	fex.drawJpg(buffer, length);
}