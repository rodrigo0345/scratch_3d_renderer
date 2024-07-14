#include "triangle.h"
#include "display.h"
#include "matrix.h"
#include <stdio.h>


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
