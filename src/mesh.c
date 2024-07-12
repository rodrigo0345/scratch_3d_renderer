#include "mesh.h"
#include "array.h"
#include "display.h"
#include "triangle.h"
#include <bits/floatn-common.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL, .mesh_faces = NULL, .rotation = {0, 0, 0}};

// the order of the points matters!!!
face_t mesh_faces[N_CUBE_FACES] = {
    {.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
    {.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000}, // Front face

    {.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
    {.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00}, // Right face

    {.a = 6, .b = 5, .c = 7, .color = 0xFFFF00FF},
    {.a = 6, .b = 7, .c = 8, .color = 0xFFFF00FF}, // Back face

    {.a = 8, .b = 7, .c = 2, .color = 0xFFFF00FF},
    {.a = 8, .b = 2, .c = 1, .color = 0xFFFF00FF}, // Left face

    {.a = 2, .b = 7, .c = 5, .color = 0xFFFF0000},
    {.a = 2, .b = 5, .c = 3, .color = 0xFFFF0000}, // Top face

    {.a = 6, .b = 8, .c = 1, .color = 0xFFFFF000},
    {.a = 6, .b = 1, .c = 4, .color = 0xFFFFF000} // Bottom face
};


vec3_t mesh_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 0
    {.x = -1, .y = 1, .z = -1},  // 1
    {.x = 1, .y = 1, .z = -1},   // 2
    {.x = 1, .y = -1, .z = -1},  // 3
    {.x = 1, .y = 1, .z = 1},    // 4
    {.x = 1, .y = -1, .z = 1},   // 5
    {.x = -1, .y = 1, .z = 1},   // 6
    {.x = -1, .y = -1, .z = 1}   // 7
};

void load_cube_mesh_data(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    array_push(mesh.vertices, mesh_vertices[i]);
  }

  for (int i = 0; i < N_CUBE_FACES; i++) {
    array_push(mesh.mesh_faces, mesh_faces[i]);
  }
}

void free_resources(void) {
  if (mesh.vertices == NULL || mesh.mesh_faces == NULL)
    return;

  array_free(mesh.vertices);
  array_free(mesh.mesh_faces);
  free(color_buffer);
}

void load_obj_file_data(const char *filename) {
  FILE *file;
  file = fopen(filename, "r");
  char line[1024];

  if (mesh.vertices != NULL || mesh.mesh_faces != NULL) {
    array_free(mesh.vertices);
    array_free(mesh.mesh_faces);
    mesh.mesh_faces = NULL;
    mesh.vertices = NULL;
  }

  while (fgets(line, 1024, file)) {
    // Vertex information
    if (strncmp(line, "v ", 2) == 0) {
      vec3_t vertex;
      sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(mesh.vertices, vertex);
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
      face_t face = {.a = vertex_indices[0],
                     .b = vertex_indices[1],
                     .c = vertex_indices[2]};
      array_push(mesh.mesh_faces, face);
    }
  }
}
