#ifndef LIGHT_H
#define LIGHT_H

#include "display.h"
#include "vector.h"

typedef struct {
  vec3_t direction;
} light_t;

color_t light_apply_intensity(color_t original_color, float percentage_factor);
triangle_3d_t apply_directional_light(triangle_3d_t triangle, light_t light);

#endif
