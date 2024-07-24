#include "camera.h"
#include "vector.h"

static camera_t camera = {.position = {.x = 0, .y = 0, .z = 0},
                          .direction = {.x = 0, .y = 0, .z = 1},
                          .forward_velocity = {.x = 0, .y = 0, .z = 0},
                          .yaw_angle = .0f,
                          .pitch_angle = .0f};


void set_camera(vec4_t yaw_direction, vec4_t pitch_direction){
  camera.direction = vec3_from_vec4(pitch_direction);
}

const camera_t get_camera(void) { return camera; }

void camera_go_forward(float delta_time) {
  camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
  camera.position = vec3_add(camera.position, camera.forward_velocity);
}

void camera_go_back(float delta_time) {
  camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
  camera.position = vec3_sub(camera.position, camera.forward_velocity);
}

void camera_yaw_left(float angle, float delta_time) {
  camera.yaw_angle -= angle * delta_time;
}

void camera_yaw_right(float angle, float delta_time) {
  camera.yaw_angle += angle * delta_time;
}

void camera_pitch_up(float angle, float delta_time) {
  camera.pitch_angle -= angle * delta_time;
}

void camera_pitch_down(float angle, float delta_time) {
  camera.pitch_angle += angle * delta_time;
}

void camera_go_up(float delta_time) { camera.position.y += 5.0 * delta_time; }
void camera_go_down(float delta_time) { camera.position.y -= 5.0 * delta_time; }
