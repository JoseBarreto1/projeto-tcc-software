#include "face_detect.h"
#include "face_recognition.h"
#include "display.h"
#include "debug.h"

static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config_t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 60; // 80 default
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}
mtmn_config_t mtmn_config = app_mtmn_config();

box_array_t *run_face_detect(camera_fb_t *fb, dl_matrix3du_t *image_matrix)
{
  fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
  return face_detect(image_matrix, &mtmn_config);
}

void clean_box_face_detect(box_array_t *box_face_detect, dl_matrix3du_t *image_matrix)
{
  free(box_face_detect->score);
  free(box_face_detect->box);
  free(box_face_detect->landmark);
  free(box_face_detect);

  dl_matrix3du_free(image_matrix);
}

bool processing_face(camera_fb_t *frame_buffer, face_id_list *id_list, int *user_number, bool *enroll_enabled, callback_face_detect callback)
{
  if (frame_buffer != NULL) 
	{
		esp_camera_fb_return(frame_buffer);
		frame_buffer = NULL;
	}
  delay(100);

  frame_buffer = esp_camera_fb_get();
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, frame_buffer->width, frame_buffer->height, 3);
  box_array_t *box_face_detect = NULL;
  box_face_detect = run_face_detect(frame_buffer, image_matrix);

  if (box_face_detect)
  {
    display_write_string("Processando ... ");
    print("Rosto detectado");

    int id_face = run_face_recognition(id_list, image_matrix, box_face_detect, user_number, enroll_enabled);

    clean_box_face_detect(box_face_detect, image_matrix);

    if (id_face >= 0)
    {
      callback();
    }

    return true;
  }
  else
  {
    dl_matrix3du_free(image_matrix);
    return false;
  }
}