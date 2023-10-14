#include "face_recognition.h"

int run_face_recognition(face_id_list *id_list, dl_matrix3du_t *image_matrix, box_array_t *net_boxes, long user_number, bool *enroll_enabled)
{
	dl_matrix3du_t *aligned_face = NULL;
	int matched_id = -1;

	if (MAX_NUMBER_USER == user_number)
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
			int8_t id_example = enroll_face(id_list, aligned_face);

			save_user(id_example, aligned_face);

			// Serial.printf("Inscrevendo-se Face ID: %d Exemplo: %d\n", id_list.tail, id_example);
			if (id_example == 0)
			{
				*enroll_enabled = false;
				matched_id = 0;
			}
		}
		else
		{ // reconhecimento
			matched_id = recognize_face(id_list, aligned_face);
		}
	}

	dl_matrix3du_free(aligned_face);
	return matched_id;
}