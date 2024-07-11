#include "mesh.h"
#include "array.h"
#include "display.h"
#include "triangle.h"
#include <bits/floatn-common.h>
#include <math.h>
#include <stdio.h>

mesh_t mesh = {.vertices = NULL, .mesh_faces = NULL, .rotation = {0, 0, 0}};

// the order of the points matters!!!
face_t mesh_faces[N_CUBE_FACES] = {
    {.a = 0, .b = 1, .c = 2}, {.a = 0, .b = 2, .c = 3}, // Front face

    {.a = 3, .b = 2, .c = 4}, {.a = 3, .b = 4, .c = 5}, // Right face

    {.a = 5, .b = 4, .c = 6}, {.a = 5, .b = 6, .c = 7}, // Back face

    {.a = 7, .b = 6, .c = 1}, {.a = 7, .b = 1, .c = 0}, // Left face

    {.a = 1, .b = 6, .c = 4}, {.a = 1, .b = 4, .c = 2}, // Top face

    {.a = 5, .b = 7, .c = 0}, {.a = 5, .b = 0, .c = 3} // Bottom face
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
  array_free(mesh.vertices);
  array_free(mesh.mesh_faces);
  free(color_buffer);
}

void load_obj_file_data(const char *filepath) {
  FILE *fd;

  if ((fd = fopen(filepath, "r")) == NULL) {
    fprintf(stderr, "Obj file: %s not found!", filepath);
    return;
  }

  fprintf(stdout, "Object loaded: %s", filepath);

  char *line_buffer = malloc(sizeof(char) * 1024);
  do {
    fgets(line_buffer, 1024, fd);
    if (line_buffer == NULL)
      break;

    char first_character = line_buffer[0];

    switch (first_character) {
    case 'v':
      // load vertex
      vec3_t vertex = {.x = .0f, .y = .0f, .z = .0f};

      sscanf(line_buffer, "%*c %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(mesh.vertices, vertex);
      break;
    case 'f':
      // load vertex index
      face_t vertex_index = {.a = 0, .b = 0, .c = 0};

      sscanf(line_buffer, "%*c %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
             &vertex_index.a, &vertex_index.b, &vertex_index.c);

      array_push(mesh.mesh_faces, vertex_index);

      break;
      break;
    default:
      continue;
    }
  } while (!feof(fd));

  fclose(fd);
}
