#include "face_detect.h"
#include "fr_forward.h"
#include "user.h"

#define MAX_NUMBER_USER 3
#define ESP_OK          0

int run_face_recognition(face_id_list *id_list, dl_matrix3du_t *image_matrix, box_array_t *net_boxes, long user_number, bool * enroll_enabled);