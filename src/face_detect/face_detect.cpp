#include "face_detect.h"

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

box_array_t * run_face_detect(camera_fb_t * fb, dl_matrix3du_t *image_matrix)
{
	fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
	return face_detect(image_matrix, &mtmn_config);
}

void clean_box_face_detect(box_array_t * box_face_detect, dl_matrix3du_t *image_matrix)
{
	free(box_face_detect->score);
    free(box_face_detect->box);
    free(box_face_detect->landmark);
    free(box_face_detect);

	dl_matrix3du_free(image_matrix);
}