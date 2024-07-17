#include "triangle.h"
#include "display.h"
#include "texture.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

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

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  // Start x_start and x_end from the top vertex (x0,y0)
  float x_start = x0;
  float x_end = x0;

  // Loop all the scanlines from top to bottom
  for (int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);
    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
  float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

  // Start x_start and x_end from the bottom vertex (x2,y2)
  float x_start = x2;
  float x_end = x2;

  // Loop all the scanlines from bottom to top
  for (int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);
    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

void draw_triangle(triangle_2d_t triangle, uint32_t color,
                   Draw_mode draw_mode) {

  int x0 = triangle.points[0].x, y0 = triangle.points[0].y;
  int x1 = triangle.points[1].x, y1 = triangle.points[1].y;
  int x2 = triangle.points[2].x, y2 = triangle.points[2].y;

  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
  }

  if (draw_mode == WIRE || draw_mode == WIRE_DOT || draw_mode == SOLID_WIRE) {
    draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
              triangle.points[1].y, color - 0xFF);
    draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x,
              triangle.points[2].y, color - 0xFF);
    draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x,
              triangle.points[0].y, color - 0xFF);
    if (draw_mode == WIRE)
      return;
  }

  if (draw_mode == WIRE_DOT) {
    // TODO:
    return;
  }

  // para evitar dividir por 0
  if (y1 == y2) {
    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
  } else if (y0 == y1) {
    // Draw flat-top triangle
    fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
  } else {
    // Calculate the new vertex (Mx,My) using triangle similarity
    int My = y1;
    int Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, Mx, My, x1, y1, color);

    // Draw flat-top triangle
    fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
  }
}

float triangle_area(vec2_t a, vec2_t b, vec2_t c) {
  return fabs(vec2_dot(vec2_sub(b, a), vec2_sub(c, a))) / 2.0;
}

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

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position x and y using interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_texel(int x, int y, uint32_t *texture, vec2_t point_a, vec2_t point_b,
                vec2_t point_c, float u0, float v0, float u1, float v1,
                float u2, float v2) {
  vec2_t point_p = {x, y};
  vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);

  float alpha = weights.x;
  float beta = weights.y;
  float gamma = weights.z;

  // Perform the interpolation of all U and V values using barycentric weights
  float interpolated_u = (u0)*alpha + (u1)*beta + (u2)*gamma;
  float interpolated_v = (v0)*alpha + (v1)*beta + (v2)*gamma;

  // Map the UV coordinate to the full texture width and height
  int tex_x = abs((int)(interpolated_u * texture_width));
  int tex_y = abs((int)(interpolated_v * texture_height));

  draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
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
void draw_textured_triangle(triangle_2d_t triangle, uint32_t *texture,
                            Draw_mode draw_mode) {
  int x0 = triangle.points[0].x, y0 = triangle.points[0].y;
  int x1 = triangle.points[1].x, y1 = triangle.points[1].y;
  int x2 = triangle.points[2].x, y2 = triangle.points[2].y;

  int u0 = triangle.texcoords[0].u, v0 = triangle.texcoords[0].v;
  int u1 = triangle.texcoords[1].u, v1 = triangle.texcoords[1].v;
  int u2 = triangle.texcoords[2].u, v2 = triangle.texcoords[2].v;

  // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&u0, &u1, sizeof(float));
    swap(&v0, &v1, sizeof(float));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
    swap(&u1, &u2, sizeof(float));
    swap(&v1, &v2, sizeof(float));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
    swap(&u0, &u1, sizeof(float));
    swap(&v0, &v1, sizeof(float));
  }

  // Create vector points and texture coords after we sort the vertices
  vec2_t point_a = {x0, y0};
  vec2_t point_b = {x1, y1};
  vec2_t point_c = {x2, y2};

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
        draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2,
                   v2);
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
        draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2,
                   v2);
      }
    }
  }

  if(draw_mode == TEXTURED_WIRE){
    draw_line(x0, y0, x1, y1, 0xFFFFFFFF);
    draw_line(x0, y0, x2, y2, 0xFFFFFFFF);
    draw_line(x1, y1, x2, y2, 0xFFFFFFFF);
  }
}
