#include "fd_forward.h"
#include "fr_forward.h"
#include "esp_camera.h"

typedef void (*callback_face_detect)();

bool processing_face(camera_fb_t *frame_buffer, face_id_list *id_list, int *user_number, bool *enroll_enabled, callback_face_detect callback);

box_array_t * run_face_detect(camera_fb_t * fb, dl_matrix3du_t *image_matrix);

void clean_box_face_detect(box_array_t * box_face_detect, dl_matrix3du_t *image_matrix);