#include "face_recognition.h"

int run_face_recognition(face_id_list *id_list, dl_matrix3du_t *image_matrix, box_array_t *net_boxes, int *user_number, bool *enroll_enabled)
{
	dl_matrix3du_t *aligned_face = NULL;
	int matched_id = -1;

	if (MAX_NUMBER_USER == *user_number)
	{
		return matched_id;
	}

	aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

	if (!aligned_face)
	{
		// Serial.println("Não foi possível alocar buffer de reconhecimento facial");
		return matched_id;
	}
	if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK)
	{
		if (*enroll_enabled)
		{ // cadastrando rosto
			int8_t number_file = enroll_face(id_list, aligned_face);

			int next_number = *user_number + 1;
			save_user(number_file, aligned_face, next_number);

			// Serial.printf("Inscrevendo-se #: %d\n", number_file);
			if (number_file == 0)
			{
				*user_number = next_number;
				*enroll_enabled = false;
				matched_id = 0;
			}
		}
		else
		{
			matched_id = recognize_face(id_list, aligned_face);
		}
	}

	dl_matrix3du_free(aligned_face);
	return matched_id;
}

void init_face_id(face_id_list *list, int total_user)
{
	face_id_init(list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
	load_user(list, total_user);
}