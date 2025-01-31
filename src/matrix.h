#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
  float m[4][4];
} mat4_t;

typedef enum { X, Y, Z } axis;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation(float angle, axis axis);
mat4_t mat4_make_perpective(float fov, float aspect_ration, float znear,
                            float zfar);

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);

#endif
