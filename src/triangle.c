#include "triangle.h"
#include "display.h"
#include "texture.h"
#include "upng.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

///////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights alpha, beta, and gamma for point p
///////////////////////////////////////////////////////////////////////////////
//
//         (B)
//         /|\
//        / | \
//       /  |  \
//      /  (P)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  (A)------------(C)
//
///////////////////////////////////////////////////////////////////////////////
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
  // Find the vectors between the vertices ABC and point p
  vec2_t ac = vec2_sub(c, a);
  vec2_t ab = vec2_sub(b, a);
  vec2_t ap = vec2_sub(p, a);
  vec2_t pc = vec2_sub(c, p);
  vec2_t pb = vec2_sub(b, p);

  // Compute the area of the full parallegram/triangle ABC using 2D cross
  // product
  float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

  // Alpha is the area of the small parallelogram/triangle PBC divided by the
  // area of the full parallelogram/triangle ABC
  float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

  // Beta is the area of the small parallelogram/triangle APC divided by the
  // area of the full parallelogram/triangle ABC
  float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

  // Weight gamma is easily found since barycentric coordinates always add up
  // to 1.0
  float gamma = 1 - alpha - beta;

  vec3_t weights = {alpha, beta, gamma};
  return weights;
}

// get normal takes always the first point as the "middle" point
// to calculate the normal of a triangle
vec3_t get_normal(triangle_3d_t t) {
  vec3_t vector_a = vec3_from_vec4(t.points[0]); /*   A   */
  vec3_t vector_b = vec3_from_vec4(t.points[1]); /*  / \  */
  vec3_t vector_c = vec3_from_vec4(t.points[2]); /* C---B */

  // Get the vector subtraction of B-A and C-A
  vec3_t vector_ab = vec3_sub(vector_b, vector_a);
  vec3_t vector_ac = vec3_sub(vector_c, vector_a);

  vec3_normalize(&vector_ab);
  vec3_normalize(&vector_ac);

  // Compute the face normal (using cross product to find perpendicular)
  vec3_t normal = vec3_cross(vector_ab, vector_ac);

  // Normalize the resulting normal vector
  vec3_normalize(&normal);

  return normal;
}

void draw_triangle(triangle_2d_t triangle, uint32_t color,
                   Draw_mode draw_mode) {
  int x0 = triangle.points[0].x, y0 = triangle.points[0].y;
  float z0 = triangle.points[0].z, w0 = triangle.points[0].w;

  int x1 = triangle.points[1].x, y1 = triangle.points[1].y;
  float z1 = triangle.points[1].z, w1 = triangle.points[1].w;

  int x2 = triangle.points[2].x, y2 = triangle.points[2].y;
  float z2 = triangle.points[2].z, w2 = triangle.points[2].w;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
    swap(&z1, &z2, sizeof(float));
    swap(&w1, &w2, sizeof(float));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));
  }

  if (draw_mode == WIRE || draw_mode == SOLID_WIRE) {
    draw_line(x0, y0, x1, y1, 0xFFFFFFFF);
    draw_line(x1, y1, x2, y2, 0xFFFFFFFF);
    draw_line(x2, y2, x0, y0, 0xFFFFFFFF);

    if (draw_mode == WIRE)
      return;
  }

  // Create vector points and texture coords after we sort the vertices
  vec4_t point_a = {x0, y0, z0, w0};
  vec4_t point_b = {x1, y1, z1, w1};
  vec4_t point_c = {x2, y2, z2, w2};

  float inv_w_a = 1 / point_a.w;
  float inv_w_b = 1 / point_b.w;
  float inv_w_c = 1 / point_c.w;

  ///////////////////////////////////////////////////////
  // Render the upper part of the triangle (flat-bottom)
  ///////////////////////////////////////////////////////
  float inv_slope_1 = 0;
  float inv_slope_2 = 0;

  if (y1 - y0 != 0)
    inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
  if (y2 - y0 != 0)
    inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

  if (y1 - y0 != 0) {
    for (int y = y0; y <= y1; y++) {
      int x_start = x1 + (y - y1) * inv_slope_1;
      int x_end = x0 + (y - y0) * inv_slope_2;

      if (x_end < x_start) {
        swap(&x_start, &x_end,
             sizeof(int)); // swap if x_start is to the right of x_end
      }

      for (int x = x_start; x < x_end; x++) {
        vec3_t weights = barycentric_weights(
            (vec2_t){.x = point_a.x, .y = point_a.y},
            (vec2_t){.x = point_b.x, .y = point_b.y},
            (vec2_t){.x = point_c.x, .y = point_c.y}, (vec2_t){.x = x, .y = y});

        float alpha = weights.x;
        float beta = weights.y;
        float gamma = weights.z;
        float interpolated_reciprocal_w =
            1.0 - (inv_w_a * alpha + inv_w_b * beta + inv_w_c * gamma);

        if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
          // Draw our pixel with the color that comes from the texture
          draw_pixel(x, y, color);
          update_zbuffer_at(x, y, interpolated_reciprocal_w);
        }
      }
    }
  }

  ///////////////////////////////////////////////////////
  // Render the bottom part of the triangle (flat-top)
  ///////////////////////////////////////////////////////
  inv_slope_1 = 0;
  inv_slope_2 = 0;

  if (y2 - y1 != 0)
    inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
  if (y2 - y0 != 0)
    inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

  if (y2 - y1 != 0) {
    for (int y = y1; y <= y2; y++) {
      int x_start = x1 + (y - y1) * inv_slope_1;
      int x_end = x0 + (y - y0) * inv_slope_2;

      if (x_end < x_start) {
        swap(&x_start, &x_end,
             sizeof(int)); // swap if x_start is to the right of x_end
      }
      for (int x = x_start; x < x_end; x++) {
        vec3_t weights = barycentric_weights(
            (vec2_t){.x = point_a.x, .y = point_a.y},
            (vec2_t){.x = point_b.x, .y = point_b.y},
            (vec2_t){.x = point_c.x, .y = point_c.y}, (vec2_t){.x = x, .y = y});

        float alpha = weights.x;
        float beta = weights.y;
        float gamma = weights.z;
        float interpolated_reciprocal_w =
            1.0 - (inv_w_a * alpha + inv_w_b * beta + inv_w_c * gamma);

        if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
          // Draw our pixel with the color that comes from the texture
          draw_pixel(x, y, color);
          // set_z_buffer(x, y, interpolated_reciprocal_w);
        }
      }
    }
  }

  if (draw_mode == TEXTURED_WIRE) {
    draw_line(x0, y0, x1, y1, 0xFFFFFFFF);
    draw_line(x0, y0, x2, y2, 0xFFFFFFFF);
    draw_line(x1, y1, x2, y2, 0xFFFFFFFF);
  }
}

float triangle_area(vec2_t a, vec2_t b, vec2_t c) {
  return fabs(vec2_dot(vec2_sub(b, a), vec2_sub(c, a))) / 2.0;
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position x and y using interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_texel(int x, int y, upng_t *texture, vec4_t point_a, vec4_t point_b,
                vec4_t point_c, float u0, float v0, float u1, float v1,
                float u2, float v2) {

  int texture_height = upng_get_height(texture);
  int texture_width = upng_get_width(texture);

  vec2_t point_p = {x, y};
  vec3_t weights =
      barycentric_weights((vec2_t){.x = point_a.x, .y = point_a.y},
                          (vec2_t){.x = point_b.x, .y = point_b.y},
                          (vec2_t){.x = point_c.x, .y = point_c.y}, point_p);

  float alpha = weights.x;
  float beta = weights.y;
  float gamma = weights.z;

  float interpolated_u;
  float interpolated_v;
  float interpolated_reciprocal_w;

  // Perform the interpolation of all U and V values using barycentric weights
  interpolated_u = (u0 / point_a.w) * alpha + (u1 / point_b.w) * beta +
                   (u2 / point_c.w) * gamma;
  interpolated_v = (v0 / point_a.w) * alpha + (v1 / point_b.w) * beta +
                   (v2 / point_c.w) * gamma;

  // Also interpolate the value of 1/w for this pixel
  interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta +
                              (1 / point_c.w) * gamma;

  interpolated_u /= interpolated_reciprocal_w;
  interpolated_v /= interpolated_reciprocal_w;

  // Map the UV coordinate to the full texture width and height
  int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
  int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

  interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

  if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
    uint32_t *tex_buffer = (uint32_t *)upng_get_buffer(texture);

    draw_pixel(x, y, tex_buffer[(texture_width * tex_y) + tex_x]);

    // update z buffer with 1/w
    update_zbuffer_at(x, y, interpolated_reciprocal_w);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(triangle_2d_t triangle, Draw_mode draw_mode) {
  int x0 = triangle.points[0].x, y0 = triangle.points[0].y;
  float z0 = triangle.points[0].z, w0 = triangle.points[0].w;

  int x1 = triangle.points[1].x, y1 = triangle.points[1].y;
  float z1 = triangle.points[1].z, w1 = triangle.points[1].w;

  int x2 = triangle.points[2].x, y2 = triangle.points[2].y;
  float z2 = triangle.points[2].z, w2 = triangle.points[2].w;

  float u0 = triangle.texcoords[0].u, v0 = triangle.texcoords[0].v;
  float u1 = triangle.texcoords[1].u, v1 = triangle.texcoords[1].v;
  float u2 = triangle.texcoords[2].u, v2 = triangle.texcoords[2].v;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));

    swap(&u0, &u1, sizeof(float));
    swap(&v0, &v1, sizeof(float));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
    swap(&z1, &z2, sizeof(float));
    swap(&w1, &w2, sizeof(float));

    swap(&u1, &u2, sizeof(float));
    swap(&v1, &v2, sizeof(float));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));

    swap(&u0, &u1, sizeof(float));
    swap(&v0, &v1, sizeof(float));
  }

  // Create vector points and texture coords aft we sort the vertices
  vec4_t point_a = {x0, y0, z0, w0};
  vec4_t point_b = {x1, y1, z1, w1};
  vec4_t point_c = {x2, y2, z2, w2};

  vec2_t a_uv = {u0, 1 - v0};
  vec2_t b_uv = {u1, 1 - v1};
  vec2_t c_uv = {u2, 1 - v2};

  ///////////////////////////////////////////////////////
  // Render the upper part of the triangle (flat-bottom)
  ///////////////////////////////////////////////////////
  float inv_slope_1 = 0;
  float inv_slope_2 = 0;

  if (y1 - y0 != 0)
    inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
  if (y2 - y0 != 0)
    inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

  if (y1 - y0 != 0) {
    for (int y = y0; y <= y1; y++) {
      int x_start = x1 + (y - y1) * inv_slope_1;
      int x_end = x0 + (y - y0) * inv_slope_2;

      if (x_end < x_start) {
        swap(&x_start, &x_end,
             sizeof(int)); // swap if x_start is to the right of x_end
      }

      for (int x = x_start; x < x_end; x++) {
        // Draw our pixel with the color that comes from the texture
        draw_texel(x, y, triangle.texture, point_a, point_b, point_c, a_uv.x,
                   a_uv.y, b_uv.x, b_uv.y, c_uv.x, c_uv.y);
      }
    }
  }

  ///////////////////////////////////////////////////////
  // Render the bottom part of the triangle (flat-top)
  ///////////////////////////////////////////////////////
  inv_slope_1 = 0;
  inv_slope_2 = 0;

  if (y2 - y1 != 0)
    inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
  if (y2 - y0 != 0)
    inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

  if (y2 - y1 != 0) {
    for (int y = y1; y <= y2; y++) {
      int x_start = x1 + (y - y1) * inv_slope_1;
      int x_end = x0 + (y - y0) * inv_slope_2;

      if (x_end < x_start) {
        swap(&x_start, &x_end,
             sizeof(int)); // swap if x_start is to the right of x_end
      }

      for (int x = x_start; x < x_end; x++) {
        // Draw our pixel with the color that comes from the texture
        draw_texel(x, y, triangle.texture, point_a, point_b, point_c, a_uv.x,
                   a_uv.y, b_uv.x, b_uv.y, c_uv.x, c_uv.y);
      }
    }
  }

  if (draw_mode == TEXTURED_WIRE) {
    draw_line(x0, y0, x1, y1, 0xFFFFFFFF);
    draw_line(x0, y0, x2, y2, 0xFFFFFFFF);
    draw_line(x1, y1, x2, y2, 0xFFFFFFFF);
  }
}

bool is_top_left(vec2_t *start, vec2_t *end) {
  vec2_t edge = {end->x - start->x, end->y - start->y};
  bool is_top_edge = edge.y == 0 && edge.x > 0;
  bool is_left_edge = edge.y < 0;
  return is_top_edge || is_left_edge;
}

float edge_cross(vec2_t *a, vec2_t *b, vec2_t *p) {
  vec2_t ab = {b->x - a->x, b->y - a->y};
  vec2_t ap = {p->x - a->x, p->y - a->y};
  return (float)(ab.x * ap.y - ab.y * ap.x);
}

void triangle_fill_optimized(triangle_2d_t t, uint32_t color) {
  vec2_t v0 = (vec2_t){.x = t.points[0].x, .y = t.points[0].y};
  vec2_t v1 = (vec2_t){.x = t.points[1].x, .y = t.points[1].y};
  vec2_t v2 = (vec2_t){.x = t.points[2].x, .y = t.points[2].y};

  int x_min = floor(MIN(MIN(v0.x, v1.x), v2.x));
  int y_min = floor(MIN(MIN(v0.y, v1.y), v2.y));
  int x_max = ceil(MAX(MAX(v0.x, v1.x), v2.x));
  int y_max = ceil(MAX(MAX(v0.y, v1.y), v2.y));

  // Compute the constant delta_s that will be used for the horizontal and
  // vertical steps
  float delta_w0_col = (v1.y - v2.y);
  float delta_w1_col = (v2.y - v0.y);
  float delta_w2_col = (v0.y - v1.y);

  float delta_w0_row = (v2.x - v1.x);
  float delta_w1_row = (v0.x - v2.x);
  float delta_w2_row = (v1.x - v0.x);

  // Compute the area of the entire triangle/parallelogram
  float area = edge_cross(&v0, &v1, &v2);

  // Fill convention (top-left rasterization rule)
  float bias0 = is_top_left(&v1, &v2) ? 0 : -1;
  float bias1 = is_top_left(&v2, &v0) ? 0 : -1;
  float bias2 = is_top_left(&v0, &v1) ? 0 : -1;

  vec2_t p0 = {x_min + .5f, y_min + .5f};
  float w0_row = edge_cross(&v1, &v2, &p0) + -0.00001;
  float w1_row = edge_cross(&v2, &v0, &p0) + -0.00001;
  float w2_row = edge_cross(&v0, &v1, &p0) + -0.00001;

  // Loop all candidate pixels inside the bounding box
  for (int y = y_min; y <= y_max; y++) {
    float w0 = w0_row;
    float w1 = w1_row;
    float w2 = w2_row;
    for (int x = x_min; x <= x_max; x++) {
      bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;

      if (is_inside) {
        // Compute the barycentric coordinates
        float alpha = (float)w0 / area;
        float beta = (float)w1 / area;
        float gamma = (float)w2 / area;

        // Interpolate the reciprocal of w
        float interpolated_reciprocal_w =
            1 - (alpha * (1 / (float)t.points[0].w) +
                 beta * (1 / (float)t.points[1].w) +
                 gamma * (1 / (float)t.points[2].w));

        if (interpolated_reciprocal_w == -NAN) {
          printf("A: %f\n", alpha);
          printf("B: %f\n", beta);
          printf("G: %f\n", gamma);
        }

        // Check if the current pixel is in front
        if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
          draw_pixel(x, y, color);
          update_zbuffer_at(x, y, interpolated_reciprocal_w);
        }
      }
      w0 += delta_w0_col;
      w1 += delta_w1_col;
      w2 += delta_w2_col;
    }
    w0_row += delta_w0_row;
    w1_row += delta_w1_row;
    w2_row += delta_w2_row;
  }
}

void draw_texel_optimized(int x, int y, upng_t *texture, vec4_t point_a,
                          vec4_t point_b, vec4_t point_c, float u0, float v0,
                          float u1, float v1, float u2, float v2, float alpha,
                          float beta, float gamma,
                          float interpolated_reciprocal_w) {
  // Perform the interpolation of all U and V values using barycentric weights
  float interpolated_u =
      (u0 * alpha + u1 * beta + u2 * gamma) / interpolated_reciprocal_w;
  float interpolated_v =
      (v0 * alpha + v1 * beta + v2 * gamma) / interpolated_reciprocal_w;

  int texture_width = upng_get_width(texture);
  int texture_height = upng_get_height(texture);

  // Map the UV coordinate to the full texture width and height
  int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
  int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

  if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
    uint32_t *tex_buffer = (uint32_t *)upng_get_buffer(texture);
    draw_pixel(x, y, tex_buffer[(texture_width * tex_y) + tex_x]);
    update_zbuffer_at(x, y, interpolated_reciprocal_w);
  }
}

void triangle_fill_texture_optimized(triangle_2d_t t, uint32_t color) {
  int x0 = t.points[0].x, y0 = t.points[0].y;
  float z0 = t.points[0].z, w0 = t.points[0].w;

  int x1 = t.points[1].x, y1 = t.points[1].y;
  float z1 = t.points[1].z, w1 = t.points[1].w;

  int x2 = t.points[2].x, y2 = t.points[2].y;
  float z2 = t.points[2].z, w2 = t.points[2].w;

  float u0 = t.texcoords[0].u, v0_aux = t.texcoords[0].v;
  float u1 = t.texcoords[1].u, v1_aux = t.texcoords[1].v;
  float u2 = t.texcoords[2].u, v2_aux = t.texcoords[2].v;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));

    swap(&u0, &u1, sizeof(float));
    swap(&v0_aux, &v1_aux, sizeof(float));
    swap(&t.points[0], &t.points[1], sizeof(vec4_t));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
    swap(&z1, &z2, sizeof(float));
    swap(&w1, &w2, sizeof(float));

    swap(&u1, &u2, sizeof(float));
    swap(&v1_aux, &v2_aux, sizeof(float));
    swap(&t.points[1], &t.points[2], sizeof(vec4_t));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&z0, &z1, sizeof(float));
    swap(&w0, &w1, sizeof(float));

    swap(&u0, &u1, sizeof(float));
    swap(&v0_aux, &v1_aux, sizeof(float));
    swap(&t.points[0], &t.points[1], sizeof(vec4_t));
  }

  vec2_t v0 = {t.points[0].x, t.points[0].y};
  vec2_t v1 = {t.points[1].x, t.points[1].y};
  vec2_t v2 = {t.points[2].x, t.points[2].y};

  int x_min = floor(MIN(MIN(v0.x, v1.x), v2.x));
  int y_min = floor(MIN(MIN(v0.y, v1.y), v2.y));
  int x_max = ceil(MAX(MAX(v0.x, v1.x), v2.x));
  int y_max = ceil(MAX(MAX(v0.y, v1.y), v2.y));

  // Compute the constant delta_s that will be used for the horizontal and
  // vertical steps
  float delta_w0_col = (v1.y - v2.y);
  float delta_w1_col = (v2.y - v0.y);
  float delta_w2_col = (v0.y - v1.y);

  float delta_w0_row = (v2.x - v1.x);
  float delta_w1_row = (v0.x - v2.x);
  float delta_w2_row = (v1.x - v0.x);

  // Compute the area of the entire triangle/parallelogram
  float area = edge_cross(&v0, &v1, &v2);

  // Fill convention (top-left rasterization rule)
  float bias0 = is_top_left(&v1, &v2) ? 0 : -0.0001;
  float bias1 = is_top_left(&v2, &v0) ? 0 : -0.0001;
  float bias2 = is_top_left(&v0, &v1) ? 0 : -0.0001;

  vec2_t p0 = {x_min + 0.5f, y_min + 0.5f};
  float w0_row = edge_cross(&v1, &v2, &p0) + bias0;
  float w1_row = edge_cross(&v2, &v0, &p0) + bias1;
  float w2_row = edge_cross(&v0, &v1, &p0) + bias2;

  // Loop all candidate pixels inside the bounding box
  for (int y = y_min; y <= y_max; y++) {
    float w0 = w0_row;
    float w1 = w1_row;
    float w2 = w2_row;
    for (int x = x_min; x <= x_max; x++) {
      bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
      if (is_inside) {
        // Compute the barycentric coordinates
        float alpha = w0 / area;
        float beta = w1 / area;
        float gamma = w2 / area;

        // Interpolate the reciprocal of w
        float interpolated_reciprocal_w = alpha * (1 / t.points[0].w) +
                                          beta * (1 / t.points[1].w) +
                                          gamma * (1 / t.points[2].w);

        draw_texel_optimized(x, y, t.texture, t.points[0], t.points[1],
                             t.points[2], t.texcoords[1].u, t.texcoords[0].v,
                             t.texcoords[1].u, t.texcoords[1].v,
                             t.texcoords[2].u, t.texcoords[2].v, alpha, beta,
                             gamma, interpolated_reciprocal_w);
      }
      w0 += delta_w0_col;
      w1 += delta_w1_col;
      w2 += delta_w2_col;
    }
    w0_row += delta_w0_row;
    w1_row += delta_w1_row;
    w2_row += delta_w2_row;
  }
}
