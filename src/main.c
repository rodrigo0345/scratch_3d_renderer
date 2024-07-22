#include "array.h"
#include "camera.h"
#include "clipping.h"
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

bool is_running = true;
float delta_time = 0.0f;

mat4_t proj_matrix;
mat4_t view_matrix;

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
  float aspect_ratio_y = window_height / (float)window_width;
  float aspect_ratio_x = window_width / (float)window_height;
  float fovy = 3.141549 / 3;                                  // 60 deg
  float fovx = atan(tan(fovy / 2.0) * aspect_ratio_x) * 2.0f; // 60 deg
  float znear = 0.1f;
  float zfar = 100.0f;
  proj_matrix = mat4_make_perpective(fovy, aspect_ratio_y, znear, zfar);

  initialize_frustum_planes(fovx, fovy, znear, zfar);

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
    }
    if (event.key.keysym.sym == SDLK_w) {
      x_translate += 1;
    }
    if (event.key.keysym.sym == SDLK_s) {
      x_translate -= 1;
    }
    if (event.key.keysym.sym == SDLK_1) {
      // display wireframe and small dot
      mode = WIRE_DOT;
    }
    if (event.key.keysym.sym == SDLK_2) {
      // wireframe lines
      mode = WIRE;
    }
    if (event.key.keysym.sym == SDLK_3) {
      // solid
      mode = SOLID;
    }
    if (event.key.keysym.sym == SDLK_4) {
      // solid and wire
      mode = SOLID_WIRE;
    }
    if (event.key.keysym.sym == SDLK_5) {
      // solid and wire
      mode = TEXTURED;
    }
    if (event.key.keysym.sym == SDLK_6) {
      // solid and wire
      mode = TEXTURED_WIRE;
    }
    // if (event.key.keysym.sym == SDLK_c) {
    //   // back-face culling
    //   culling = ON;
    // }
    // if (event.key.keysym.sym == SDLK_d) {
    //   // no back-face culling
    //   culling = OFF;
    // }
    if (event.key.keysym.sym == SDLK_f) {
      const char *filepath = "assets/f22.obj";
      // const char* filepath = "assets/cube.obj";
      // const char* filepath = "assets/tank.obj";
      load_png_texture_data("assets/f22.png");
      load_obj_file_data(filepath);
    }
    if (event.key.keysym.sym == SDLK_k) {
      const char *filepath = "assets/cube.obj";
      // const char* filepath = "assets/tank.obj";
      load_png_texture_data("assets/cube.png");
      load_obj_file_data(filepath);
    }
    if (event.key.keysym.sym == SDLK_t) {
      const char *filepath = "assets/drone.obj";
      load_png_texture_data("assets/drone.png");
      load_obj_file_data(filepath);
    }

    // camera movement
    if (event.key.keysym.sym == SDLK_w) {
      camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
      camera.position = vec3_add(camera.position, camera.forward_velocity);
    }
    if (event.key.keysym.sym == SDLK_s) {
      camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
      camera.position = vec3_sub(camera.position, camera.forward_velocity);
    }
    if (event.key.keysym.sym == SDLK_a) {
      camera.yaw_angle -= 1.0 * delta_time;
    }
    if (event.key.keysym.sym == SDLK_d) {
      camera.yaw_angle += 1.0 * delta_time;
    }
    if (event.key.keysym.sym == SDLK_LSHIFT) {
      camera.position.y += 1.5 * delta_time;
    }
    if (event.key.keysym.sym == SDLK_LCTRL) {
      camera.position.y -= 1.5 * delta_time;
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
  // int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() -
  // previous_frame_time);
  num_triangles_to_render = 0;

  // // Only delay execution if we are running too fast
  // if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
  //   SDL_Delay(time_to_wait);
  // }

  // precisa de ser convertido para segundos, há de ficar tipo 0.001s
  delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;

  previous_frame_time = SDL_GetTicks();

  // mesh.rotation.y += .6f * delta_time;
  mesh.translation.z = 5;

  // mesh.rotation.z += 0.01;
  // mesh.rotation.x = -3.1416 / 3;
  // mesh.scale.y += 0.001;

  // mesh.translation.y += 0.01 * delta_time;
  // mesh.translation.x += 1.00 * delta_time;
  //

  vec3_t up_direction = {0, 1, 0};
  vec3_t target = {0, 0, 1};

  mat4_t camera_yaw_rotation = mat4_make_rotation(camera.yaw_angle, Y);
  vec4_t direction = mat4_mul_vec4(
      camera_yaw_rotation,
      (vec4_t){.x = target.x, .y = target.y, .z = target.z, .w = 0.0f});
  camera.direction =
      (vec3_t){.x = direction.x, .y = direction.y, .z = direction.z};

  target = vec3_add(camera.position, camera.direction);

  // create view matrix
  view_matrix = mat4_look_at(camera.position, target, up_direction);

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
      transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

      // Save transformed vertex in the array of transformed vertices
      transformed_triangle.points[j] = transformed_vertex;
    }

    vec3_t normal = get_normal(transformed_triangle);

    if (culling != OFF) {

      // // Check backface culling
      vec3_t vector_a =
          vec3_from_vec4(transformed_triangle.points[0]); /*   A   */

      // Find the vector between a point in the triangle and the camera origin
      // a camera está sempre na origem
      vec3_t origin = {0, 0, 0};
      vec3_t camera_ray = vec3_sub(origin, vector_a);

      // Calculate how aligned the camera ray is with the face normal (using dot
      // product)
      float dot_normal_camera = vec3_dot(normal, camera_ray);

      // Bypass the triangles that are looking away from the camera
      if (dot_normal_camera < 0) {
        continue;
      }
    }

    // clipping
    polygon_t polygon = create_polygon_from_triangle(
        vec3_from_vec4(transformed_triangle.points[0]),
        vec3_from_vec4(transformed_triangle.points[1]),
        vec3_from_vec4(transformed_triangle.points[2]), mesh_face.a_uv,
        mesh_face.b_uv, mesh_face.c_uv);

    clip_polygon(&polygon);

    triangle_3d_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
    int num_of_triangles_after_clipping = 0;

    triangles_from_polygon(&polygon, triangles_after_clipping,
                           &num_of_triangles_after_clipping);

    for (int t = 0; t < num_of_triangles_after_clipping; t++) {

      vec4_t projected_points[3];
      triangle_3d_t triangle_after_clipping = triangles_after_clipping[t];

      // Loop all three vertices to perform projection
      for (int j = 0; j < 3; j++) {
        // Project the current vertex
        projected_points[j] = mat4_mul_vec4_project(
            proj_matrix, triangle_after_clipping.points[j]);

        triangles_after_clipping[t].color = transformed_triangle.color;

        // scale into the view
        projected_points[j].x *= window_width / 2.0f;
        projected_points[j].y *= window_height / 2.0f;

        // Invert the y values to account for flipped screen y coordenate
        projected_points[j].y *= -1;

        // translate the projected points to the middle of the screen
        projected_points[j].x += (int)(window_width / 2.0f);
        projected_points[j].y += (int)(window_height / 2.0f);
      }

      transformed_triangle =
          apply_directional_light(triangles_after_clipping[t], light);

      triangle_2d_t projected_triangle = {
          .points =
              {
                  projected_points[0],
                  projected_points[1],
                  projected_points[2],
              },
          .texcoords =
              {
                  {.u = triangle_after_clipping.texcoords[0].u,
                   .v = triangle_after_clipping.texcoords[0].v},
                  {.u = triangle_after_clipping.texcoords[1].u,
                   .v = triangle_after_clipping.texcoords[1].v},
                  {.u = triangle_after_clipping.texcoords[2].u,
                   .v = triangle_after_clipping.texcoords[2].v},
              },
          .color = transformed_triangle.color};

      if (num_triangles_to_render >= MAX_TRIANGLES_PER_MESH) {
        continue;
      }

      // Save the projected triangle in the array of triangles to render
      triangles_to_render[num_triangles_to_render++] = projected_triangle;
    }
  }
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
