#include "vector.h"
#include <math.h>

vec3_t vec3_rotate_x(vec3_t v, float angle) {
  vec3_t rotated = {
      .x = v.x,
      .y = v.y * cos(angle) - v.z * sin(angle),
      .z = v.y * sin(angle) + v.z * cos(angle),
  };
  return rotated;
}
vec3_t vec3_rotate_y(vec3_t v, float angle) {
  vec3_t rotated = {
      .x = v.x * cos(angle) - v.z * sin(angle),
      .y = v.y,
      .z = v.x * sin(angle) + v.z * cos(angle),
  };
  return rotated;
}
vec3_t vec3_rotate_z(vec3_t v, float angle) {
  vec3_t rotated = {.x = v.x * cos(angle) - v.y * sin(angle),
                    .y = v.x * sin(angle) + v.y * cos(angle),
                    .z = v.z};
  return rotated;
}

float vec2_length(vec2_t v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

vec2_t vec2_add(vec2_t a, vec2_t b){
  return (vec2_t){
    .x = a.x + b.x,
    .y = a.y + b.y,
  };
}

vec2_t vec2_sub(vec2_t a, vec2_t b){
  return (vec2_t){
    .x = a.x - b.x,
    .y = a.y - b.y,
  };
}

vec2_t vec2_mul(vec2_t a, float scale){
  return (vec2_t){
    .x = a.x * scale,
    .y = a.y * scale,
  };
}
vec2_t vec2_div(vec2_t a, float scale){
  return (vec2_t){
    .x = a.x / scale,
    .y = a.y / scale,
  };
}

float vec2_dot(vec2_t a, vec2_t b){
  return (a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(vec2_t* v){
  float len = vec2_length(*v);
  v = &(vec2_t) {
    .x = v->x / len,
    .y = v->y / len,
  };
}

////////////////////////////////
// Vec 3
///////////////////////////////
float vec3_length(vec3_t v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec3_t vec3_add(vec3_t a, vec3_t b){
  return (vec3_t){
    .x = a.x + b.x,
    .y = a.y + b.y,
    .z = a.z + b.z,
  };
}

vec3_t vec3_sub(vec3_t a, vec3_t b){
  return (vec3_t){
    .x = a.x - b.x,
    .y = a.y - b.y,
    .z = a.z - b.z,
  };
}

vec3_t vec3_mul(vec3_t a, float scale){
  return (vec3_t){
    .x = a.x * scale,
    .y = a.y * scale,
    .z = a.z * scale,
  };
}

vec3_t vec3_div(vec3_t a, float scale){
  return (vec3_t){
    .x = a.x / scale,
    .y = a.y / scale,
    .z = a.z / scale,
  };
}

// only works with vec3
vec3_t vec3_cross(vec3_t a, vec3_t b){
  return (vec3_t){
    .x = a.y * b.z - a.z * b.y,
    .y = a.z * b.x - a.x * b.z,
    .z = a.x * b.y - a.y * b.x
  };
}

float vec3_dot(vec3_t a, vec3_t b){
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(vec3_t* v){
  float len = vec3_length(*v);
  v = &(vec3_t) {
    .x = v->x / len,
    .y = v->y / len,
    .z = v->z / len,
  };
}
