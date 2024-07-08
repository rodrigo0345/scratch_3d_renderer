#include "display.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "array.h"

triangle_t* triangles_to_render = NULL;

vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};
vec3_t camera_position = {.x = 0, .y = 0, .z = -10};

float fov_factor = 640;

bool is_running = true;

uint32_t previous_frame_time = 0;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_height * window_width);

  if (!color_buffer) {
    fprintf(stderr, "Error initializing frame buffer");
    return;
  }

  ////////////////////////////////////////////////////////////////
  // buffer texture is going to be responsible for
  // translating our color_buffer to SDL
  ////////////////////////////////////////////////////////////////
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
}

// this was just for fun
int x_translate = 0;

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

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
    }
    break;
  }
}

// ortographic
vec2_t project_ortographic(vec3_t point) {
  vec2_t projected_point = {
      .x = fov_factor * point.x,
      .y = fov_factor * point.y,
  };
  return projected_point;
}

// perspective
vec2_t project(vec3_t point) {
  vec2_t projected_point = {
      .x = (fov_factor * point.x) / point.z,
      .y = (fov_factor * point.y) / point.z,
  };
  return projected_point;
}

vec3_t translate(vec3_t point, int x, int y, int z) {
  point.x += x;
  point.y += y;
  point.z += z;
  return point;
}

void update(void) {

  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  // sleep
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // initializa the array of triangles to render
  triangles_to_render = NULL;

  cube_rotation.y += 0.01;
  cube_rotation.x += 0.01;
  cube_rotation.z += 0.01;

  for (int i = 0; i < N_MESH_FACES; i++) {
    face_t curr_face = mesh_faces[i];
    vec3_t face_vertices[3] = {
        mesh_vertices[curr_face.a],
        mesh_vertices[curr_face.b],
        mesh_vertices[curr_face.c],
    };

    triangle_t projected_triangle;

    // Apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
      transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
      transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

      // we need to rotate before translate because
      // the center of the rotation is always {0, 0, 0}
      transformed_vertex =
          translate(transformed_vertex, x_translate, 0, -1 * camera_position.z);

      vec2_t projected_point = project(transformed_vertex);

      projected_point.x += (int)(window_width / 2);
      projected_point.y += (int)(window_height / 2);

      projected_triangle.points[j] = projected_point;
    }

    array_push(triangles_to_render, projected_triangle);
  }

  // for (int i = 0; i < N_POINTS; i++) {
  //   vec3_t point = cube_points[i];

  //   point = vec3_rotate_y(point, cube_rotation.y);
  //   point = vec3_rotate_x(point, cube_rotation.x);
  //   point = vec3_rotate_z(point, cube_rotation.z);

  //   // we need to rotate before translate because
  // // the center of the rotation is always {0, 0, 0}
  //   point = translate(point, x_translate, 0, -1 * camera_position.z);

  //   const vec2_t projected_point = project(point);

  //   // save projected point
  //   projected_points[i] = projected_point;
  // }
}

void render(void) {

  // Loop all projected triangles and render them
  const int num_triangles = array_length(triangles_to_render);

  for (int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];
    draw_triangle(triangle, true, 0xFFFF0000);
  }

  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

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

  return 0;
}
