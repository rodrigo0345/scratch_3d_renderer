#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

typedef struct {
  int a;
  int b;
  int c;
  uint32_t color;
} face_t;

typedef struct {
  vec2_t points[3];
  uint32_t color;
  float avg_depth;
} triangle_2d_t;

// this represents the triangle before
// projection
typedef struct {
  vec4_t points[3];
  uint32_t color;
} triangle_3d_t;

vec3_t get_normal(triangle_3d_t t);


#endif
