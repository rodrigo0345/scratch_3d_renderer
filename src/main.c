#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_TRIANGLES_PER_MESH 10000

triangle_2d_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

vec3_t camera_position = {.x = 0, .y = 0, .z = 0};

bool is_running = true;

mat4_t proj_matrix;

uint32_t previous_frame_time = 0;

light_t light = {.direction = {
                     .x = 0.3,
                     .y = 0,
                     .z = 1,
                 }};

int mouse_x = .0f;
int mouse_y = .0f;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_height * window_width);

  z_buffer = (float *)malloc(sizeof(float) * window_height * window_width);

  if (!color_buffer) {
    fprintf(stderr, "Error initializing frame buffer");
    return;
  }

  ////////////////////////////////////////////////////////////////
  // buffer texture is going to be responsible for
  // translating our color_buffer to SDL
  ////////////////////////////////////////////////////////////////
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  // init the perspective matrix
  float fov = 3.141549 / 3; // 60 deg
  float aspect_ratio = window_height / (float)window_width;
  float znear = 0.1f;
  float zfar = 100.0f;
  proj_matrix = mat4_make_perpective(fov, aspect_ratio, znear, zfar);

  // load texture data manually
  // mesh_texture = ;
  // texture_width = 64;
  // texture_height = 64;

  const char *filepath = "assets/efa.obj";

  // const char* filepath = "assets/cube.obj";
  // const char* filepath = "assets/tank.obj";
  //
  load_obj_file_data(filepath);
  // load_cube_mesh_data();

  load_png_texture_data("assets/efa.png");
}

// this was just for fun
int x_translate = 0;

Draw_mode mode = WIRE_DOT;
Culling_mode culling = ON;

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);
  SDL_GetMouseState(&mouse_x, &mouse_y);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      is_running = false;
    } else if (event.key.keysym.sym == SDLK_w) {
      x_translate += 1;
    } else if (event.key.keysym.sym == SDLK_s) {
      x_translate -= 1;
    } else if (event.key.keysym.sym == SDLK_1) {
      // display wireframe and small dot
      mode = WIRE_DOT;
    } else if (event.key.keysym.sym == SDLK_2) {
      // wireframe lines
      mode = WIRE;
    } else if (event.key.keysym.sym == SDLK_3) {
      // solid
      mode = SOLID;
    } else if (event.key.keysym.sym == SDLK_4) {
      // solid and wire
      mode = SOLID_WIRE;
    } else if (event.key.keysym.sym == SDLK_5) {
      // solid and wire
      mode = TEXTURED;
    } else if (event.key.keysym.sym == SDLK_6) {
      // solid and wire
      mode = TEXTURED_WIRE;
    } else if (event.key.keysym.sym == SDLK_c) {
      // back-face culling
      culling = ON;
    } else if (event.key.keysym.sym == SDLK_d) {
      // no back-face culling
      culling = OFF;
    } else if (event.key.keysym.sym == SDLK_f) {
      const char *filepath = "assets/f22.obj";
      // const char* filepath = "assets/cube.obj";
      // const char* filepath = "assets/tank.obj";
      load_png_texture_data("assets/f22.png");
      load_obj_file_data(filepath);
    } else if (event.key.keysym.sym == SDLK_k) {
      const char *filepath = "assets/cube.obj";
      // const char* filepath = "assets/tank.obj";
      load_png_texture_data("assets/cube.png");
      load_obj_file_data(filepath);
    } else if (event.key.keysym.sym == SDLK_t) {
      const char *filepath = "assets/drone.obj";
      load_png_texture_data("assets/drone.png");
      load_obj_file_data(filepath);
    }
    break;
  }
}

vec3_t translate(vec3_t point, int x, int y, int z) {
  point.x += x;
  point.y += y;
  point.z += z;
  return point;
}

void update(void) {
  // Wait some time until the reach the target frame time in milliseconds
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
  num_triangles_to_render = 0;

  // Only delay execution if we are running too fast
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  mesh.rotation.y += .01f;

  // mesh.rotation.z += 0.01;
  mesh.rotation.x = -3.1416 / 3;
  // mesh.scale.y += 0.001;

  mesh.translation.z = 5;

  mat4_t scale_matrix =
      mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

  mat4_t translation_matrix = mat4_make_translation(
      mesh.translation.x, mesh.translation.y, mesh.translation.z);

  mat4_t rotation_matrix_x = mat4_make_rotation(mesh.rotation.x, X);
  mat4_t rotation_matrix_y = mat4_make_rotation(mesh.rotation.y, Y);
  mat4_t rotation_matrix_z = mat4_make_rotation(mesh.rotation.z, Z);

  // Loop all triangle faces of our mesh
  int num_faces = array_length(mesh.mesh_faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.mesh_faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a];
    face_vertices[1] = mesh.vertices[mesh_face.b];
    face_vertices[2] = mesh.vertices[mesh_face.c];

    triangle_3d_t transformed_triangle = {.points = {},
                                          .color = mesh_face.color};

    // Loop all three vertices of this current face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      // create a world matrix
      mat4_t world_matrix = mat4_identity();
      world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
      world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

      // Save transformed vertex in the array of transformed vertices
      transformed_triangle.points[j] = transformed_vertex;
    }

    vec3_t normal = get_normal(transformed_triangle);

    if (culling != OFF) {

      // // Check backface culling
      vec3_t vector_a =
          vec3_from_vec4(transformed_triangle.points[0]); /*   A   */

      // Find the vector between a point in the triangle and the camera origin
      vec3_t camera_ray = vec3_sub(camera_position, vector_a);

      // Calculate how aligned the camera ray is with the face normal (using dot
      // product)
      float dot_normal_camera = vec3_dot(normal, camera_ray);

      // Bypass the triangles that are looking away from the camera
      if (dot_normal_camera < 0) {
        continue;
      }
    }

    vec4_t projected_points[3];

    // Loop all three vertices to perform projection
    for (int j = 0; j < 3; j++) {
      // Project the current vertex
      projected_points[j] =
          mat4_mul_vec4_project(proj_matrix, transformed_triangle.points[j]);

      // scale into the view
      projected_points[j].x *= window_width / 2.0f;
      projected_points[j].y *= window_height / 2.0f;

      // Invert the y values to account for flipped screen y coordenate
      projected_points[j].y *= -1;

      // translate the projected points to the middle of the screen
      projected_points[j].x += (int)(window_width / 2.0f);
      projected_points[j].y += (int)(window_height / 2.0f);
    }

    transformed_triangle = apply_directional_light(transformed_triangle, light);

    triangle_2d_t projected_triangle = {
        .points =
            {
                projected_points[0],
                projected_points[1],
                projected_points[2],
            },
        .texcoords =
            {
                {.u = mesh_face.a_uv.u, .v = mesh_face.a_uv.v},
                {.u = mesh_face.b_uv.u, .v = mesh_face.b_uv.v},
                {.u = mesh_face.c_uv.u, .v = mesh_face.c_uv.v},
            },
        .color = transformed_triangle.color};

    if(num_triangles_to_render >= MAX_TRIANGLES_PER_MESH){
      continue;
    }

    // Save the projected triangle in the array of triangles to render
    triangles_to_render[num_triangles_to_render++] = projected_triangle;
  }

  // // TODO: sort the triangles to render with the avg_depth (Painter's Algo)
  // const int triangles_len = array_length(triangles_to_render);
  // for (int i = 0; i < triangles_len; i++) {
  //   for (int j = i; j < triangles_len; j++) {
  //     if (triangles_to_render[i].avg_depth <
  //     triangles_to_render[j].avg_depth) {
  //       swap(&triangles_to_render[i], &triangles_to_render[j],
  //            sizeof(triangle_2d_t));
  //     }
  //   }
  // }
}

void render(void) {

  for (int i = 0; i < MAX_TRIANGLES_PER_MESH; i++) {
    triangle_2d_t triangle = triangles_to_render[i];

    if (mode == TEXTURED || mode == TEXTURED_WIRE) {
      draw_textured_triangle(triangle, mesh_texture, mode);
    } else {
      draw_triangle(triangle, triangle.color, mode);
    }

    triangles_to_render[i] = (triangle_2d_t){
      .color = 0x000000,
    };
  }

  render_color_buffer();
  clear_color_buffer(0xFF000000);
  clear_z_buffer();

  SDL_RenderPresent(renderer);
}

int main(void) {
  is_running = initialize_window();

  setup();
  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();
  free_resources();

  return 0;
}
