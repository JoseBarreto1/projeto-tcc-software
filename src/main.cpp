#include "face_recognition.h"
#include "face_detect.h"
#include "constants.h"
#include "display.h"
#include "camera.h"
#include "debug.h"
#include "timer.h"
#include "user.h"

// variaveis globais
static face_id_list id_list = {0};
static bool enroll_enabled = true;
static int user_number = 0;
static int selected_option = MENU_OPTION;
camera_fb_t *fb = NULL;

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
  delay(200);

  camera_init();

  init_sensor_camera();

  init_SPIFFS();

  user_number = find_last_number_user_save();
  init_face_id(&id_list, user_number);
}

void return_menu(void *arg)
{
  selected_option = MENU_DEFAULT;
  delay(100);
  if (fb != NULL)
  {
    esp_camera_fb_return(fb);
    fb = NULL;
  }
  digitalWrite(FLASH_PIN, LOW);
  display_error();
  selected_option = MENU_OPTION;
  return;
}

void open_door()
{
  if (digitalRead(RELAY_PIN) == LOW)
  {
    digitalWrite(RELAY_PIN, HIGH);
    print("Porta Destrancada");
    delay(400); // Debounce
    digitalWrite(RELAY_PIN, LOW);
  }
}

void success_face_detected()
{
  print("--------- Chegou ----------");
  digitalWrite(FLASH_PIN, LOW);
  selected_option = MENU_OPTION;
  display_success();
  open_door();
}

void menu_option()
{
  display_menu();

  if (digitalRead(PUSH_BUTTON_1) == HIGH)
  {
    init_timer(TIME_DEFAULT, &return_menu);

    display_write_string(" Enquadre seu  \n \n   Rosto na  \n \n  Tela ", TEXT_SIZE_DEFAULT, TFT_MAROON);
    delay(300);
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

void login()
{
  if (!processing_face(fb, &id_list, &user_number, &enroll_enabled, &success_face_detected) && selected_option != MENU_DEFAULT)
  {
    camera_live_stream(fb);
  }
}

void registration()
{
  bool push_button_up = digitalRead(PUSH_BUTTON_1) == HIGH;
  bool push_button_down = digitalRead(PUSH_BUTTON_2) == HIGH;
  bool is_correct = enter_password(push_button_up, push_button_down);

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

void loop()
{
  switch (selected_option)
  {
  case MENU_OPTION:
    menu_option();
    break;

  case LOGIN_OPTION:
    login();
    break;

  case REGISTER_OPTION:
    registration();
    break;

  default:
    break;
  }
}
