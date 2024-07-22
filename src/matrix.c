#include "matrix.h"
#include "vector.h"
#include <math.h>

mat4_t mat4_identity(void) {
  return (mat4_t){.m = {
                      {1, 0, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 1},
                  }};
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
  mat4_t m = mat4_identity();
  m.m[0][0] = sx;
  m.m[1][1] = sy;
  m.m[2][2] = sz;
  return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
  mat4_t m = mat4_identity();
  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;
  return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
  vec4_t result;
  float *ptr = (float *)&result;
  float tmp[] = {v.x, v.y, v.z, v.w};

  for (int i = 0; i < 4; i++) {
    float line_res = 0;
    for (int j = 0; j < 4; j++) {
      line_res += m.m[i][j] * tmp[j];
    }
    ptr[i] = line_res;
  }
  return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
  mat4_t result = {0}; // Initialize all elements to zero
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result.m[i][j] = 0; // Initialize the element to zero before accumulation
      for (int k = 0; k < 4; k++) {
        result.m[i][j] += a.m[i][k] * b.m[k][j];
      }
    }
  }
  return result;
}

mat4_t mat4_make_rotation(float angle, axis axis) {
  float c = cos(angle);
  float s = sin(angle);
  mat4_t m = mat4_identity();
  switch (axis) {
  case X:
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    break;
  case Y:
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    break;
  case Z:
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    break;
  }
  return m;
}

mat4_t mat4_make_perpective(float fov, float aspect_ration, float znear,
                            float zfar) {
  mat4_t m = {{{0}}};

  m.m[0][0] = aspect_ration * (1 / tan(fov / 2));
  m.m[1][1] = 1 / tan(fov / 2);
  m.m[2][2] = zfar / (zfar - znear);
  m.m[2][3] = (-zfar * znear) / (zfar - znear);
  m.m[3][2] = 1.0f;

  return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
  vec4_t result = mat4_mul_vec4(mat_proj, v);

  if (result.w != 0.0) {
    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;
  }

  return result;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {

  // forward vector
  vec3_t z = vec3_sub(target, eye);
  vec3_normalize(&z);

  // right vector dá um vetor perpendicular entre up e z
  vec3_t x = vec3_cross(up, z);
  vec3_normalize(&x);

  // up vector, que é a perpendicular entre z e x
  vec3_t y = vec3_cross(z, x);

  return (mat4_t) {{
      {x.x, x.y, x.z, -vec3_dot(x, eye)},
      {y.x, y.y, y.z, -vec3_dot(y, eye)},
      {z.x, z.y, z.z, -vec3_dot(z, eye)},
      {0, 0, 0, 1},
  }};
}
