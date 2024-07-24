#include "mesh.h"
#include "array.h"
#include "display.h"
#include "texture.h"
#include "triangle.h"
#include <bits/floatn-common.h>
#include <stdio.h>
#include <string.h>

#define MAX_MESH_NUM 10
static mesh_t meshes[MAX_MESH_NUM];
static int mesh_count = 0;

void free_resources(void) {
  mesh_t mesh;
  for (int i = 0; i < mesh_count; i++) {
    mesh = meshes[i];
    if (mesh.vertices == NULL || mesh.mesh_faces == NULL)
      return;

    array_free(mesh.vertices);
    array_free(mesh.mesh_faces);
  }
  mesh_count = 0;
  free(get_color_buffer());
}

int get_num_meshes(void) { return mesh_count; }

mesh_t* get_mesh(int mesh_index){
  return &meshes[mesh_index];
}

void load_mesh(char *obj_path, char *tex_path, vec3_t scale, vec3_t translation,
               vec3_t rotation) {
  load_mesh_obj_data(&meshes[mesh_count], obj_path);
  load_mesh_png_data(&meshes[mesh_count], tex_path);

  meshes[mesh_count].scale = scale;
  meshes[mesh_count].translation = translation;
  meshes[mesh_count].rotation = rotation;

  mesh_count++;
}

void load_mesh_png_data(mesh_t *mesh, char *filename) {
  upng_t *png_image = upng_new_from_file(filename);
  if (png_image != NULL) {
    upng_decode(png_image);
    if (upng_get_error(png_image) == UPNG_EOK) {
      mesh->texture = png_image;
    }
  }
}

void load_mesh_obj_data(mesh_t *mesh, const char *filename) {
  FILE *file;
  file = fopen(filename, "r");
  char line[1024];

  if (mesh->vertices != NULL || mesh->mesh_faces != NULL) {
    array_free(mesh->vertices);
    array_free(mesh->mesh_faces);
    array_free(mesh->tex_coords);
    mesh->mesh_faces = NULL;
    mesh->vertices = NULL;
    mesh->tex_coords = NULL;
  }

  // Falta carregar as uv coords dos obj
  while (fgets(line, 1024, file)) {
    // Vertex information
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t vertex;
      sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(mesh->vertices, vertex);
    }

    if (strncmp(line, "vt ", 3) == 0) {
      tex2_t texture_coords;
      sscanf(line, "vt %f %f", &texture_coords.u, &texture_coords.v);
      array_push(mesh->tex_coords, texture_coords);
    }

    // Face information
    if (strncmp(line, "f ", 2) == 0) {
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];
      sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
             &texture_indices[0], &normal_indices[0], &vertex_indices[1],
             &texture_indices[1], &normal_indices[1], &vertex_indices[2],
             &texture_indices[2], &normal_indices[2]);
      face_t face = {.a = vertex_indices[0] - 1,
                     .b = vertex_indices[1] - 1,
                     .c = vertex_indices[2] - 1,
                     .a_uv = mesh->tex_coords[texture_indices[0] - 1],
                     .b_uv = mesh->tex_coords[texture_indices[1] - 1],
                     .c_uv = mesh->tex_coords[texture_indices[2] - 1],
                     .color = 0xFFFFFFFF};
      array_push(mesh->mesh_faces, face);
    }
  }
}
