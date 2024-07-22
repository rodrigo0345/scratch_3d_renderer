
#include "clipping.h"
#include "texture.h"
#include "vector.h"
#include <math.h>

plane_t frustum_planes[6];

void initialize_frustum_planes(float fovx, float fovy, float z_near,
                               float z_far) {
  float cos_half_fov_x = cos(fovx / 2);
  float sin_half_fov_x = sin(fovx / 2);

  float cos_half_fov_y = cos(fovy / 2);
  float sin_half_fov_y = sin(fovy / 2);

  vec3_t origin = {0, 0, 0};

  frustum_planes[LEFT_FRUSTUM_PLANE].point = origin;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

  frustum_planes[RIGHT_FRUSTUM_PLANE].point = origin;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

  frustum_planes[TOP_FRUSTUM_PLANE].point = origin;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

  frustum_planes[BOTTOM_FRUSTUM_PLANE].point = origin;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

  frustum_planes[NEAR_FRUSTUM_PLANE].point.x = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].point.y = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].point.z = z_near;
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

  frustum_planes[FAR_FRUSTUM_PLANE].point.x = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].point.y = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].point.z = z_far;
  frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2,
                                       tex2_t a_uv, tex2_t b_uv, tex2_t c_uv) {
  polygon_t polygon = {.vertices = {v0, v1, v2},
                       .texture = {a_uv, b_uv, c_uv},
                       .num_vertices = 3};
  return polygon;
}

float float_lerp(float a, float b, float t) { return a + t * (b - a); }

void clip_polygon_against_plane(polygon_t *polygon, int plane) {
  vec3_t plane_point = frustum_planes[plane].point;
  vec3_t plane_normal = frustum_planes[plane].normal;

  vec3_t inside_vertices[MAX_NUM_POLY_VERTIVES];
  tex2_t inside_texcoords[MAX_NUM_POLY_VERTIVES];

  int num_inside_vertives = 0;

  vec3_t *current_vertex = &polygon->vertices[0];
  vec3_t *previous_vertex = &polygon->vertices[polygon->num_vertices - 1];

  tex2_t *current_texcoord = &polygon->texture[0];
  tex2_t *previous_texcoord = &polygon->texture[polygon->num_vertices - 1];

  float current_dot = 0;
  float previous_dot =
      vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

  while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
    current_dot =
        vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

    if (current_dot * previous_dot < 0) {
      // TODO: find the intersection point
      float t = previous_dot / (previous_dot - current_dot);

      // a magia esta aqui
      vec3_t intersection_point = {
          .x = float_lerp(previous_vertex->x, current_vertex->x, t),
          .y = float_lerp(previous_vertex->y, current_vertex->y, t),
          .z = float_lerp(previous_vertex->z, current_vertex->z, t),
      };

      // use the lurp formula
      tex2_t interpolated_texcoord = {
          .u = float_lerp(previous_texcoord->u, current_texcoord->u, t),
          .v = float_lerp(previous_texcoord->v, current_texcoord->v, t),
      };

      inside_vertices[num_inside_vertives] = vec3_clone(&intersection_point);
      inside_texcoords[num_inside_vertives] = interpolated_texcoord;
      num_inside_vertives++;
    }

    if (current_dot > 0) {
      inside_vertices[num_inside_vertives] = vec3_clone(current_vertex);
      inside_texcoords[num_inside_vertives] = tex2_clone(current_texcoord);
      num_inside_vertives++;
    }

    previous_dot = current_dot;
    previous_vertex = current_vertex;
    previous_texcoord = current_texcoord;

    current_vertex++;
    current_texcoord++;
  }

  // assign to the polygon
  polygon->num_vertices = num_inside_vertives;

  for (int i = 0; i < num_inside_vertives; i++) {
    polygon->vertices[i] = inside_vertices[i];
    polygon->texture[i] = tex2_clone(&inside_texcoords[i]);
  }
}

void clip_polygon(polygon_t *polygon) {
  clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void triangles_from_polygon(polygon_t *polygon,
                            triangle_3d_t triangles_after_clipping[],
                            int *num_of_triangles_after_clipping) {
  for (int i = 0; i < polygon->num_vertices - 2; i++) {
    int index0 = 0;
    int index1 = i + 1;
    int index2 = i + 2;

    triangles_after_clipping[i].points[0] =
        vec4_from_vec3(polygon->vertices[index0]);
    triangles_after_clipping[i].points[1] =
        vec4_from_vec3(polygon->vertices[index1]);
    triangles_after_clipping[i].points[2] =
        vec4_from_vec3(polygon->vertices[index2]);

    triangles_after_clipping[i].texcoords[0] = polygon->texture[index0];
    triangles_after_clipping[i].texcoords[1] = polygon->texture[index1];
    triangles_after_clipping[i].texcoords[2] = polygon->texture[index2];
  }

  *num_of_triangles_after_clipping = polygon->num_vertices - 2;
}
