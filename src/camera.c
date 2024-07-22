#include "camera.h"

camera_t camera = {
    .position = {.x = 0, .y = 0, .z = 0},
    .direction = {.x = 0, .y = 0, .z = 1},
    .forward_velocity = {.x = 0, .y = 0, .z = 0},
    .yaw_angle = .0f,
};
