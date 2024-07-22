
#ifndef CLIPPING_H
#define CLIPPING_H

#include "triangle.h"
#define MAX_NUM_POLY_VERTIVES 10
#define MAX_NUM_POLY_TRIANGLES 10

#include "vector.h"

enum {
  LEFT_FRUSTUM_PLANE,
  RIGHT_FRUSTUM_PLANE,
  TOP_FRUSTUM_PLANE,
  BOTTOM_FRUSTUM_PLANE,
  NEAR_FRUSTUM_PLANE,
  FAR_FRUSTUM_PLANE,
};

typedef struct {
  vec3_t point;
  vec3_t normal;
} plane_t;

typedef struct {
  vec3_t vertices[MAX_NUM_POLY_VERTIVES];
  tex2_t texture[MAX_NUM_POLY_VERTIVES];
  int num_vertices;
} polygon_t;

extern plane_t frustum_planes[6];

void initialize_frustum_planes(float fovx, float fovy, float z_near, float z_far);
polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);
void clip_polygon(polygon_t *polygon);
void triangles_from_polygon(polygon_t *polygon, triangle_3d_t triangles_after_clipping[],
                            int* num_of_triangles_after_clipping);

#endif
