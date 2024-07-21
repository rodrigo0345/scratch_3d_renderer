
#ifndef MESH_H
#define MESH_H

#include "texture.h"
#include "triangle.h"
#include "vector.h"

#define N_CUBE_VERTICES 8
extern vec3_t mesh_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES (6 * 2)
extern face_t mesh_faces[N_CUBE_FACES];

typedef struct {
  vec3_t *vertices;
  tex2_t *tex_coords;
  face_t *mesh_faces;

  // transformation, scale, rotate
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;

} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void free_resources(void);
void load_obj_file_data(const char *filepath);

#endif
