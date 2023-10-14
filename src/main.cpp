#include "camera.h"
#include "camera_index.h"
#include "constants.h"
#include "timer.h"
#include "face_recognition.h"

void init_face_id();

void open_door();

void return_menu(void *arg);

camera_fb_t *fb = NULL;

static face_id_list id_list = {0};

static bool enroll_enabled = true;

long user_number = 0;

static int selected_option = 0;

int password[3] = {4, 2, 3}; // Inicialize a senha com zeros
int password_temp[3] = {0, 0, 0};
int current_position = 0; // Posição do dígito atual
int count_teste = 0;

void setup()
{
  Serial.begin(115200);

  // inicializando pinos de entrada e saída
  pinMode(FLASH_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  pinMode(PUSH_BUTTON_2, INPUT_PULLUP);
  digitalWrite(FLASH_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  display_init();
  delay(4000);

  static esp_err_t cam_err = camera_init();
  if (cam_err != ESP_OK)
  {
    Serial.printf("A inicialização da câmera falhou com o seguinte erro: 0x%x", cam_err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);
  s->set_vflip(s, 1);

  init_SPIFFS();

  init_face_id();
}

void init_face_id()
{
  face_id_init(&id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
  load_user(&id_list, &user_number);
}

bool face_detected()
{
  fb = esp_camera_fb_get();
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
  box_array_t *box_face_detect = NULL;

  run_face_detect(fb, image_matrix);

  if (box_face_detect)
  {
    Serial.println("Rosto detectado");
    int id_face = run_face_recognition(&id_list, image_matrix, box_face_detect, user_number, &enroll_enabled);

    digitalWrite(FLASH_PIN, LOW);

    clean_box_face_detect(box_face_detect, image_matrix);

    esp_camera_fb_return(fb);
    fb = NULL;

    if (id_face >= 0)
    {
      display_success();
      open_door();
      delay(400);
    }
    else
    {
      display_error();
      delay(400);
      Serial.println("Nenhum rosto correspondente encontrado");
    }

    selected_option = 0;

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

void open_door()
{
  if (digitalRead(RELAY_PIN) == LOW)
  {
    digitalWrite(RELAY_PIN, HIGH);
    // Serial.println("Porta Destrancada");
    delay(400); // Debounce
    digitalWrite(RELAY_PIN, LOW);
  }
}

void camera_live_stream()
{
  fb = esp_camera_fb_get();
  display_live_stream((const uint8_t *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  fb = NULL;
}

void menu_option()
{
  display_menu();
  delay(400); // Debounce

  if (digitalRead(PUSH_BUTTON_1) == HIGH)
  {
    selected_option = 1;
    enroll_enabled = false;

    init_timer(30, &return_menu);

    display_initial_count();
    digitalWrite(FLASH_PIN, HIGH);
  }

  else if (digitalRead(PUSH_BUTTON_2) == HIGH)
  {
    selected_option = 2;
    enroll_enabled = true;

    init_timer(30, &return_menu);
  }
}

bool register_password()
{
  display_password(current_position, password_temp);

  if (digitalRead(PUSH_BUTTON_1) == HIGH)
  {
    password_temp[current_position] = (password_temp[current_position] + 1) % 10; // Incrementar o dígito
    delay(200);                                                                   // Debounce
  }

  // Ler o botão de alterar a posição do dígito
  if (digitalRead(PUSH_BUTTON_2) == HIGH)
  {
    current_position = (current_position + 1) % 3; // Avançar para a próxima posição
    delay(200);                                    // Debounce
  }

  bool is_same = true;

  // Comparar os elementos dos dois vetores
  for (int i = 0; i < 3; i++)
  {
    if (password_temp[i] != password[i])
    {
      is_same = false;
      break;
    }
  }

  if (is_same)
  {
    display_write_string("Senha correta.");
    delay(2000);
    return true;
  }
  else
  {
    // printf("Senha incorreta.\n");
    return false;
  }
}

void login()
{
  if (!face_detected())
  {
    camera_live_stream();
  }
}

void registration()
{
  bool is_correct = register_password();

  if (is_correct)
  {
    init_timer(30, &return_menu);

    display_initial_count();
    digitalWrite(FLASH_PIN, HIGH);

    login();
  }
}

void return_menu(void *arg)
{
  selected_option = 0;
}

void loop()
{
  switch (selected_option)
  {
  case 1:
    login();
    break;

  case 2:
    registration();
    break;

  default:
    menu_option();
    break;
  }
}
