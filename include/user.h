#include "fr_forward.h"

#define FACE_WIDTH 56
#define FACE_HEIGHT 56

#define FACE_ID_SAVE_NUMBER 3
#define MAX_NUMBER_USER 3
#define ENROLL_CONFIRM_TIMES 5

void save_user(int8_t number_file, dl_matrix3du_t *aligned_face, int user_number);

void load_user(face_id_list *id_list, int total_user);

void delete_user();

int find_last_number_user_save();

bool enter_password(bool push_button_up, bool push_button_down);

void delete_password();