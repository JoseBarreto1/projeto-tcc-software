#include "fr_forward.h"

void init_face_id(face_id_list *list, int total_user);

int run_face_recognition(face_id_list *id_list, dl_matrix3du_t *image_matrix, box_array_t *net_boxes, int *user_number, bool * enroll_enabled);