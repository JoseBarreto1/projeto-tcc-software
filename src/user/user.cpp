#include "user.h"

void save_user(int8_t number_file, dl_matrix3du_t *aligned_face, long * user_number)
{
	size_t last_number = (int)find_last_number_user_file(user_number);

	last_number += 1;
	*user_number = last_number;

	char *matrix_filename = (char *)malloc(33 + sizeof(number_file));
	sprintf(matrix_filename, "/spiffs/usuario_%d/d_matrix_%d.obj", last_number, number_file);

	FILE *matrix_file = fopen(matrix_filename, "wb");
	if (matrix_file != NULL)
	{
		fwrite(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
		// Serial.printf("Arquivo salvo: %s\n", matrix_filename);
		fclose(matrix_file);
	}
	free(matrix_filename);

	char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
	sprintf(buffer_filename, "/spiffs/usuario_%d/d_buffer_%d.dat", last_number, number_file);

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

void load_user(face_id_list * id_list, long * user_number)
{
	 size_t last_number = (int)find_last_number_user_file(user_number);

  for (size_t user_number = 1; user_number <= last_number; user_number++)
  {
    for (size_t number_file = 0; number_file < ENROLL_CONFIRM_TIMES; number_file++)
    {
      dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

      char *matrix_filename = (char *)malloc(33 + sizeof(size_t));
      sprintf(matrix_filename, "/spiffs/usuario_%d/d_matrix_%d.obj", user_number, number_file);
      // Serial.println(matrix_filename);

      FILE *matrix_file = fopen(matrix_filename, "rb");
      if (matrix_file != NULL)
      {
        fread(aligned_face, sizeof(dl_matrix3du_t), 1, matrix_file);
        fclose(matrix_file);
      }
      free(matrix_filename);

      char *buffer_filename = (char *)malloc(33 + sizeof(size_t));
      sprintf(buffer_filename, "/spiffs/usuario_%d/d_buffer_%d.dat", user_number, number_file);
      // Serial.println(buffer_filename);

      FILE *buffer_file = fopen(buffer_filename, "rb");
      if (matrix_file != NULL)
      {
        fread(aligned_face->item, sizeof(uc_t), FACE_WIDTH * FACE_HEIGHT * 3, buffer_file);
        fclose(buffer_file);
      }
      free(buffer_filename);

      enroll_face(id_list, aligned_face);
      // Serial.printf("Inscrevendo-se Face ID: %d Exemplo %d\n", id_list.tail, id_example);
    }
  }
}

int find_last_number_user_file(long * user_number)
{
	if (*user_number == 0)
  {
    fs::File root = SPIFFS.open("/");

    fs::File file = root.openNextFile();
    while (file)
    {
      String fileName = file.name();
      //Serial.println(fileName);
      int fromUnderscore = fileName.indexOf('_') + 1;
      int untilDot = fileName.lastIndexOf('/');
      String fileId = fileName.substring(fromUnderscore, untilDot);
      Serial.println(fileId);
      *user_number = max(*user_number, fileId.toInt());
      file = root.openNextFile();
    }
    return (int)user_number;
  }
  return (int)user_number;
}