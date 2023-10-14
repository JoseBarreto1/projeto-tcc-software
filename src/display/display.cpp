#include "display.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eFEX fex = TFT_eFEX(&tft);

void display_init()
{
	tft.begin();
	tft.setRotation(2); // 0 & 2 Portrait. 1 & 3 landscape
	display_write_string("Bem vindo");
	delay(2000);
}

void init_SPIFFS()
{
	if (!SPIFFS.begin())
	{
		//Serial.println("Falha na inicialização do SPIFFS!");
		SPIFFS.begin(true);
	}

	fex.listSPIFFS(); // Lista os arquivos do SPIFFS
}

void display_password(int current_position, int password_temp[3])
{
	char *display_text = (char *)malloc(95 + sizeof(size_t));
	sprintf(display_text, "Digite a senha: \n \n  Posicao atual: [%d]  \n \n   -- Segredo: -- \n \n  -- [ %d/ %d / %d ] --", current_position, password_temp[0], password_temp[1], password_temp[2]);

	display_write_char(display_text, 20, 55);

	free(display_text);
}

void display_write_string(String text)
{
	const char *text_c_str = text.c_str();
	display_write_char(const_cast<char *>(text_c_str), 35, 55);
}

void display_write_char(char * text, int16_t x, int16_t y)
{
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(35, 55);
	tft.setTextColor(TFT_WHITE);
	tft.setTextSize(1);
	tft.println(text);
	delay(2000);
}

void display_success()
{
	fex.drawJpgFile(SPIFFS, "/_success.jpeg", 0, 0);
	delay(400);
}

void display_error()
{
	fex.drawJpgFile(SPIFFS, "/_failure.jpeg", 0, 0);
	delay(400);
}

void display_menu()
{
	fex.drawJpgFile(SPIFFS, "/_initial.jpeg", 0, 0);
	delay(400); // Debounce
}

void display_initial_count()
{
	fex.drawJpgFile(SPIFFS, "/_count3.jpg", 0, 0);
	delay(400);
	fex.drawJpgFile(SPIFFS, "/_count2.jpg", 0, 0);
	delay(400);
	fex.drawJpgFile(SPIFFS, "/_count1.jpg", 0, 0);
	delay(400);
}

void display_live_stream(const uint8_t *buffer, size_t length)
{
	fex.drawJpg(buffer, length, 0, 6);
}