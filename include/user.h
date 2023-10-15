#include "fr_forward.h"
#include "display.h"

#define FACE_WIDTH 56
#define FACE_HEIGHT 56

#define FACE_ID_SAVE_NUMBER 3
#define MAX_NUMBER_USER 3
#define ENROLL_CONFIRM_TIMES 5

void save_user(int8_t number_file, dl_matrix3du_t *aligned_face, int user_number);

void load_user(face_id_list *id_list, int total_user);

int find_last_number_user_save();