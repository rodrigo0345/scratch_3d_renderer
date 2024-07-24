
#ifndef MESH_H
#define MESH_H

#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"

typedef struct {
  vec3_t *vertices;
  tex2_t *tex_coords;
  upng_t* texture;
  face_t *mesh_faces;

  // transformation, scale, rotate
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;

} mesh_t;

void load_cube_mesh_data(void);
void free_resources(void);
void load_mesh(char* obj_path, char* tex_path, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_mesh_obj_data(mesh_t* mesh, const char *filename);
void load_mesh_png_data(mesh_t* mesh, char *filename);

int get_num_meshes(void);
mesh_t* get_mesh(int mesh_index);
#endif
