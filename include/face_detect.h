#include "fd_forward.h"
#include "esp_camera.h"

box_array_t * run_face_detect(camera_fb_t * fb, dl_matrix3du_t *image_matrix);

void clean_box_face_detect(box_array_t * box_face_detect, dl_matrix3du_t *image_matrix);