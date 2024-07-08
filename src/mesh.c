#include "mesh.h"



// the order of the points matters!!!
face_t mesh_faces[N_MESH_FACES] = {
    {.a = 0, .b = 1, .c = 2}, {.a = 0, .b = 2, .c = 3},  // Front face

    {.a = 3, .b = 2, .c = 4}, {.a = 3, .b = 4, .c = 5},  // Right face

    {.a = 5, .b = 4, .c = 6}, {.a = 5, .b = 6, .c = 7},  // Back face

    {.a = 7, .b = 6, .c = 1}, {.a = 7, .b = 1, .c = 0},  // Left face

    {.a = 1, .b = 6, .c = 4}, {.a = 1, .b = 4, .c = 2},  // Top face

    {.a = 5, .b = 7, .c = 0}, {.a = 5, .b = 0, .c = 3}   // Bottom face
};

vec3_t mesh_vertices[N_MESH_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},  // 0
    {.x = -1, .y = 1, .z = -1},   // 1
    {.x = 1, .y = 1, .z = -1},    // 2
    {.x = 1, .y = -1, .z = -1},   // 3
    {.x = 1, .y = 1, .z = 1},     // 4
    {.x = 1, .y = -1, .z = 1},    // 5
    {.x = -1, .y = 1, .z = 1},    // 6
    {.x = -1, .y = -1, .z = 1}    // 7
};
