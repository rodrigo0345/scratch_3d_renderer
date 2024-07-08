
#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "vector.h"

#define N_CUBE_VERTICES 8
extern vec3_t mesh_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES (6 * 2)
extern face_t mesh_faces[N_CUBE_FACES];

typedef struct {
  vec3_t *vertices;
  face_t *mesh_faces;
  vec3_t rotation;
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void free_resources(void);

#endif