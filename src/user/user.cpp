#include "fr_forward.h"
#include "display.h"
#include "debug.h"
#include "user.h"

static int password[3] = {4, 2, 3};
static int password_del[3] = {9, 9, 9};
static int password_temp[3] = {0, 0, 0};
static int current_position = 0;

void save_user(int8_t number_file, dl_matrix3du_t *aligned_face, int user_number)
{
  char *matrix_filename = (char *)malloc(33 + sizeof(number_file));
  sprintf(matrix_filename, "/spiffs/usuario_%d/d_matrix_%d.obj", user_number, number_file);

  FILE *matrix_file = fopen(matrix_filename, "wb");
  if (matrix_file != NULL)
  {
    fwrite(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
    print(matrix_filename);
    fclose(matrix_file);
  }
  free(matrix_filename);

  char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
  sprintf(buffer_filename, "/spiffs/usuario_%d/d_buffer_%d.dat", user_number, number_file);

  FILE *buffer_file = fopen(buffer_filename, "wb");
  if (buffer_file != NULL)
  {
    uc_t *item = aligned_face->item;
    fwrite(item, sizeof(uc_t), FACE_WIDTH * FACE_HEIGHT * 3, buffer_file);
    print(buffer_filename);
    fclose(buffer_file);
  }
  free(buffer_filename);
}

void load_user(face_id_list *id_list, int total_user)
{
  for (size_t user_number = 1; user_number <= total_user; user_number++)
  {
    for (size_t number_file = 0; number_file < ENROLL_CONFIRM_TIMES; number_file++)
    {
      dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

      char *matrix_filename = (char *)malloc(33 + sizeof(size_t));
      sprintf(matrix_filename, "/spiffs/usuario_%d/d_matrix_%d.obj", user_number, number_file);
      print(matrix_filename);

      FILE *matrix_file = fopen(matrix_filename, "rb");
      if (matrix_file != NULL)
      {
        fread(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
        fclose(matrix_file);
      }
      free(matrix_filename);

      char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
      sprintf(buffer_filename, "/spiffs/usuario_%d/d_buffer_%d.dat", user_number, number_file);
      print(buffer_filename);

      FILE *buffer_file = fopen(buffer_filename, "rb");
      if (matrix_file != NULL)
      {
        fread(aligned_face->item, sizeof(uc_t), FACE_WIDTH * FACE_HEIGHT * 3, buffer_file);
        fclose(buffer_file);
      }
      free(buffer_filename);

      enroll_face(id_list, aligned_face);
    }
  }
}

int find_last_number_user_save()
{
  long user_number = 0;
  fs::File root = SPIFFS.open("/");

  fs::File file = root.openNextFile();
  while (file)
  {
    String fileName = file.name();
    print(fileName);
    int fromUnderscore = fileName.indexOf('_') + 1;
    int untilDot = fileName.lastIndexOf('/');
    String fileId = fileName.substring(fromUnderscore, untilDot);
    print(fileId);
    user_number = max(user_number, fileId.toInt());
    file = root.openNextFile();
  }
  return (int)user_number;
}

void delete_user()
{
  int total_user = find_last_number_user_save();
  for (size_t user_number = 1; user_number <= total_user; user_number++)
  {
    for (size_t number_file = 0; number_file < ENROLL_CONFIRM_TIMES; number_file++)
    {
      char str[33];
      char str2[33];
      snprintf(str, sizeof(str), "/usuario_%d/d_matrix_%d.obj", user_number, number_file);
      snprintf(str2, sizeof(str2), "/usuario_%d/d_buffer_%d.obj", user_number, number_file);
      SPIFFS.remove(str);
      SPIFFS.remove(str2);
    }
  }
  display_write_string(" usuarios \n  deletados ");
}

bool compare_array(int array1[3], int array2[3])
{
  bool is_same = true;

  // Compara os elementos dos dois vetores
  for (int i = 0; i < 3; i++)
  {
    if (array1[i] != array2[i])
    {
      is_same = false;
      break;
    }
  }
  return is_same;
}

bool enter_password(bool push_button_up, bool push_button_down)
{
  if (push_button_up)
  {
    password_temp[current_position] = (password_temp[current_position] + 1) % 10; // Incrementar um dígito
    delay(100);
  }

  if (push_button_down)
  {
    current_position = (current_position + 1) % 3; // Avançar para a próxima posição
    delay(100);
  }

  bool is_delete_user = compare_array(password_temp, password_del);

  if (is_delete_user)
  {
    delete_user();
  }

  bool is_password = compare_array(password_temp, password);

  if (is_password)
  {
    print("Senha correta.\n");
    display_write_string(" senha \n  correta ", TEXT_SIZE_MEDIUM, TFT_DARKGREEN);
    delay(800);
    return true;
  }
  else
  {
    print("Senha incorreta.\n");
    return false;
  }
}