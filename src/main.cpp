#include "esp_camera.h"
#include "camera_index.h"
#include "Arduino.h"
#include "fd_forward.h"
#include "fr_forward.h"

#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <TFT_eFEX.h>
TFT_eFEX fex = TFT_eFEX(&tft);

void init_face_id();

void open_door();

static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes);

static esp_err_t save_user(int8_t file_number, dl_matrix3du_t *aligned_face);

static esp_err_t load_user();

camera_fb_t *fb = NULL;
static esp_err_t cam_err;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 2

// #define RELAY_PIN 2
#define LED_PIN 4

static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config_t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 60; // 80 default
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}
mtmn_config_t mtmn_config = app_mtmn_config();

static int8_t enroll_enabled = 1;

static face_id_list id_list = {0};

void setup()
{
  Serial.begin(115200);

  pinMode(4, OUTPUT);   // initialize io4 as an output for LED flash.
  digitalWrite(4, LOW); // flash off/

  tft.begin();
  tft.setRotation(2); // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(35, 55);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.println("Bem vindo");
  delay(5000);

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

  // inicia com especificações altas para pré-alocar buffers maiores
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Inicialização da camera
  cam_err = esp_camera_init(&config);
  if (cam_err != ESP_OK)
  {
    Serial.printf("A inicialização da câmera falhou com o seguinte erro: 0x%x", cam_err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);
  s->set_vflip(s, 1);

  if (!SPIFFS.begin())
  {
    Serial.println("Falha na inicialização do SPIFFS!");
    SPIFFS.begin(true);
  }
  // Serial.println("\r\nInicialização concluída.");

  fex.listSPIFFS(); // Lista os arquivos para que você possa ver o que está no SPIFFS

  init_face_id();
}

void init_face_id()
{
  face_id_init(&id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
  load_user();
}

bool face_detected()
{
  fb = esp_camera_fb_get();
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
  box_array_t *box_face_detect = NULL;

  fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);

  box_face_detect = face_detect(image_matrix, &mtmn_config);

  if (box_face_detect)
  {
    Serial.println("Rosto detectado");
    int id_face = run_face_recognition(image_matrix, box_face_detect);

    free(box_face_detect->score);
    free(box_face_detect->box);
    free(box_face_detect->landmark);
    free(box_face_detect);

    dl_matrix3du_free(image_matrix);

    esp_camera_fb_return(fb);
    fb = NULL;

    if (id_face >= 0)
    {
      // Serial.printf("ID do rosto cadastrado: %u\n", id_face);
      open_door();
    }
    else
    {
      Serial.println("Nenhum rosto correspondente encontrado");
    }

    return true;
  }
  else
  {
    dl_matrix3du_free(image_matrix);

    esp_camera_fb_return(fb);
    fb = NULL;
    return false;
  }
}

static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes)
{
  dl_matrix3du_t *aligned_face = NULL;
  int matched_id = -1;

  aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

  if (!aligned_face)
  {
    Serial.println("Não foi possível alocar buffer de reconhecimento facial");
    return matched_id;
  }
  if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
  {
    if (enroll_enabled == 1)
    { // cadastrando rosto
      int8_t id_example = enroll_face(&id_list, aligned_face);

      save_user(id_example, aligned_face);

      Serial.printf("Inscrevendo-se Face ID: %d Exemplo: %d\n", id_list.tail, id_example);
      if (id_example == 0)
      {
        enroll_enabled = 0;
        Serial.printf("Inscrito Face ID: %d\n", id_list.tail);
      }
    }
    else
    { // reconhecimento
      matched_id = recognize_face(&id_list, aligned_face);
    }
  }
  else
  {
    Serial.println("Rosto não alinhado");
  }

  dl_matrix3du_free(aligned_face);
  return matched_id;
}

void open_door()
{
  if (digitalRead(LED_PIN) == LOW)
  {
    digitalWrite(LED_PIN, HIGH);
    // Serial.println("Porta Destrancada");
    delay(5000);
    digitalWrite(LED_PIN, LOW);
    // Serial.println("Porta Trancada");
  }
}

void smile_for_the_camera()
{
  long timer_millis = millis();
  while (millis() - timer_millis < 500)
  {
    if (!face_detected())
    {
      Serial.println("Rosto não detectado");
      return;
    }
  }
  fex.drawJpgFile(SPIFFS, "/_count3.jpg", 0, 0);
  delay(400);
  fex.drawJpgFile(SPIFFS, "/_count2.jpg", 0, 0);
  delay(400);
  fex.drawJpgFile(SPIFFS, "/_count1.jpg", 0, 0);
  delay(400);
}

static esp_err_t save_user(int8_t file_number, dl_matrix3du_t *aligned_face)
{
  char *matrix_filename = (char *)malloc(33 + sizeof(file_number));
  sprintf(matrix_filename, "/spiffs/usuario_1/d_matrix_%d.obj", file_number);

  FILE *matrix_file = fopen(matrix_filename, "wb");
  if (matrix_file != NULL)
  {
    fwrite(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
    // Serial.printf("Arquivo salvo: %s\n", matrix_filename);
    fclose(matrix_file);
  }
  free(matrix_filename);

  char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
  sprintf(buffer_filename, "/spiffs/usuario_1/d_buffer_%d.dat", file_number);

  FILE *buffer_file = fopen(buffer_filename, "wb");
  if (buffer_file != NULL)
  {
    uc_t *item = aligned_face->item;
    fwrite(item, sizeof(uc_t), FACE_WIDTH * FACE_HEIGHT * 3, buffer_file);
    // Serial.printf("Arquivo salvo: %s\n", buffer_filename);
    fclose(buffer_file);
  }
  free(buffer_filename);
}

static esp_err_t load_user()
{
  for (size_t i = 0; i < 5; i++)
  {
    dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

    char *matrix_filename = (char *)malloc(33 + sizeof(size_t));
    sprintf(matrix_filename, "/spiffs/usuario_1/d_matrix_%d.obj", i);
    // Serial.println(matrix_filename);

    FILE *matrix_file = fopen(matrix_filename, "rb");
    if (matrix_file != NULL)
    {
      fread(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
      fclose(matrix_file);
    }
    free(matrix_filename);

    char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
    sprintf(buffer_filename, "/spiffs/usuario_1/d_buffer_%d.dat", i);
    // Serial.println(buffer_filename);

    FILE *buffer_file = fopen(buffer_filename, "rb");
    if (matrix_file != NULL)
    {
      fread(aligned_face->item, sizeof(uc_t), FACE_WIDTH * FACE_HEIGHT * 3, buffer_file);
      fclose(buffer_file);
    }
    free(buffer_filename);

    int8_t id_example = enroll_face(&id_list, aligned_face);

    // Serial.printf("Inscrevendo-se Face ID: %d Exemplo %d\n", id_list.tail, id_example);
  }

  enroll_enabled = 0;
}

void loop()
{
  if (face_detected())
  {
    smile_for_the_camera();
  }
  else
  {
    fb = esp_camera_fb_get();
    fex.drawJpg((const uint8_t *)fb->buf, fb->len, 0, 6); // Desenha no display
    esp_camera_fb_return(fb);
    fb = NULL;
  }
}
