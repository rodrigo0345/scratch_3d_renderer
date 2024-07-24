#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "display.h"
#include "texture.h"
#include "vector.h"

typedef struct {
  int a;
  int b;
  int c;

  uint32_t color;

  tex2_t a_uv;
  tex2_t b_uv;
  tex2_t c_uv;
} face_t;

typedef struct {
  vec4_t points[3];
  tex2_t texcoords[3];
  upng_t *texture;
  uint32_t color;
} triangle_2d_t;

// this represents the triangle before
// projection
typedef struct {
  vec4_t points[3];
  uint32_t color;
  tex2_t texcoords[3];
} triangle_3d_t;

vec3_t get_normal(triangle_3d_t t);
void draw_triangle(triangle_2d_t triangle, uint32_t color, Draw_mode draw_mode);
void draw_textured_triangle(triangle_2d_t triangle, Draw_mode draw_mode);

#endif
