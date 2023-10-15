#include "Arduino.h"
#include "camera.h"
#include "camera_index.h"
#include "constants.h"
#include "timer.h"
#include "face_recognition.h"

void open_door();

void return_menu(void *arg);

// variaveis globais
camera_fb_t *fb = NULL;
static face_id_list id_list = {0};

static bool enroll_enabled = true;
int user_number = 0;
static int selected_option = MENU_OPTION;

int password[3] = {4, 2, 3};
int password_temp[3] = {0, 0, 0};
int current_position = 0;

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
  digitalWrite(PUSH_BUTTON_1, LOW);
  digitalWrite(PUSH_BUTTON_2, LOW);

  display_init();
  delay(400);

  camera_init();

  init_sensor_camera();
  init_SPIFFS();

  user_number = find_last_number_user_save();
  init_face_id(&id_list, user_number);
}

bool face_detected()
{
  fb = esp_camera_fb_get();
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
  box_array_t *box_face_detect = NULL;

  box_face_detect = run_face_detect(fb, image_matrix);

  if (box_face_detect)
  {
    display_write_string("Processando ... ");
    // Serial.println("Rosto detectado");
    int id_face = run_face_recognition(&id_list, image_matrix, box_face_detect, &user_number, &enroll_enabled);

    clean_box_face_detect(box_face_detect, image_matrix);

    esp_camera_fb_return(fb);
    fb = NULL;

    if (id_face >= 0)
    {
      digitalWrite(FLASH_PIN, LOW);
      selected_option = MENU_OPTION;
      display_success();
      open_door();
    }
    else
    {
      if (!enroll_enabled)
      {
        display_error();
        // Serial.println("Nenhum rosto reconhecido");
      }
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
  delay(200); // Debounce

  if (digitalRead(PUSH_BUTTON_1) == HIGH)
  {
    init_timer(TIME_DEFAULT, &return_menu);

    display_initial_count();
    digitalWrite(FLASH_PIN, HIGH);

    selected_option = LOGIN_OPTION;
    enroll_enabled = false;
    return;
  }

  else if (digitalRead(PUSH_BUTTON_2) == HIGH)
  {
    selected_option = REGISTER_OPTION;
    return;
  }
}

bool register_password()
{
  display_password(current_position, password_temp);

  if (digitalRead(PUSH_BUTTON_1) == HIGH)
  {
    password_temp[current_position] = (password_temp[current_position] + 1) % 10; // Incrementar o dígito
    delay(100);
  }

  // Ler o botão de alterar a posição do dígito
  if (digitalRead(PUSH_BUTTON_2) == HIGH)
  {
    current_position = (current_position + 1) % 3; // Avançar para a próxima posição
    delay(100);
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
    // Serial.println("Senha correta.\n");
    display_write_string(" senha \n  correta ", TEXT_SIZE_MEDIUM, TFT_DARKGREEN);
    delay(800);
    return true;
  }
  else
  {
    // Serial.println("Senha incorreta.\n");
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
    init_timer(TIME_DEFAULT, &return_menu);

    display_initial_count();
    digitalWrite(FLASH_PIN, HIGH);

    selected_option = LOGIN_OPTION;
    enroll_enabled = true;
    return;
  }
}

void return_menu(void *arg)
{
  digitalWrite(FLASH_PIN, LOW);
  selected_option = MENU_OPTION;
}

void loop()
{
  switch (selected_option)
  {
  case LOGIN_OPTION:
    login();
    break;

  case REGISTER_OPTION:
    registration();
    break;

  default:
    menu_option();
    break;
  }
}
