#include "Arduino.h"
#include "fr_forward.h"
#include "display.h"

#define FACE_WIDTH 56
#define FACE_HEIGHT 56

#define ENROLL_CONFIRM_TIMES 5

void save_user(int8_t number_file, dl_matrix3du_t *aligned_face);

void load_user(face_id_list * id_list, long * user_number);

int find_last_number_user_file(long * user_number);