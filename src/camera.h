
#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
  vec3_t position;
  vec3_t direction;
  vec3_t forward_velocity;
  float yaw_angle;
  float pitch_angle;
} camera_t;

const camera_t get_camera(void);
void set_camera(vec4_t yaw_direction, vec4_t pitch_direction);

void camera_go_forward(float delta_time);
void camera_go_back(float delta_time);
void camera_yaw_left(float angle, float delta_time);
void camera_yaw_right(float angle, float delta_time);
void camera_go_up(float delta_time);
void camera_go_down(float delta_time);
void camera_pitch_up(float angle, float delta_time);
void camera_pitch_down(float angle, float delta_time);

#endif
