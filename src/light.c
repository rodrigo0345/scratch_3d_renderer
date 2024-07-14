
#include "light.h"
#include "display.h"
#include "triangle.h"
#include "vector.h"
#include <stdint.h>

color_t light_apply_intensity(color_t original_color, float percentage_factor) {
  if(percentage_factor < 0) percentage_factor = 0;
  if(percentage_factor > 1) percentage_factor = 1;

  uint32_t a = (original_color & 0xFF000000);
  uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
  uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
  uint32_t b = (original_color & 0x000000FF) * percentage_factor;

  return (a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF));
}

triangle_3d_t apply_directional_light(triangle_3d_t triangle, light_t light) {

  vec3_t normal = get_normal(triangle);

  // Calculate how aligned the light direction is with the face normal (using
  // dot product)
  float dot_normal_light = -vec3_dot(normal, light.direction);

  // Apply light intensity to the triangle color
  triangle.color = light_apply_intensity(triangle.color, dot_normal_light);

  return triangle;
}
