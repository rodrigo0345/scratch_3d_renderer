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
#include <unistd.h>

#define MAX_TRIANGLES_PER_MESH 10000

static bool optimized_rendering = true;

triangle_2d_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running = true;
float delta_time = 0.0f;

mat4_t proj_matrix;
mat4_t view_matrix;

uint32_t previous_frame_time = 0;

int mouse_x = .0f;
int mouse_y = .0f;

void setup(void) {

  is_running = initialize_window();

  // init the perspective matrix
  float aspect_ratio_y = get_window_height() / (float)get_window_width();
  float aspect_ratio_x = get_window_width() / (float)get_window_height();
  float fovy = 3.141549 / 4;                                  // 60 deg
  float fovx = atan(tan(fovy / 2.0) * aspect_ratio_x) * 2.0f; // 60 deg
  float znear = 0.1f;
  float zfar = 100.0f;
  proj_matrix = mat4_make_perpective(fovy, aspect_ratio_y, znear, zfar);

  initialize_frustum_planes(fovx, fovy, znear, zfar);

  load_mesh("./assets/cube.obj", "./assets/cube.png", vec3_new(1, 1, 1),
            vec3_new(-2, 0, 4), vec3_new(0, 3.1416 / 2, 0));

  load_mesh("./assets/tank.obj", NULL, vec3_new(1, 1, 1), vec3_new(2, 0, 4),
            vec3_new(0, 3.1416 / 2, 0));

  load_mesh("./assets/bunny.obj", NULL, vec3_new(4, 4, 4), vec3_new(0, 0, 8),
            vec3_new(0, 0, 0));

  // load_mesh("./assets/runway.obj", "./assets/runway.png", vec3_new(1, 1, 1),
  //           vec3_new(0, -1, 16), vec3_new(0, 0, 0));
  // load_mesh("./assets/f117.obj", "./assets/f117.png", vec3_new(1, 1, 1),
  //           vec3_new(0, -1, 16), vec3_new(0, 0, 0));
  // load_mesh("./assets/crab.obj", "./assets/crab.png", vec3_new(1, 1, 1),
  //           vec3_new(-2, 0, 8), vec3_new(0, 0, 0));
  // load_mesh("./assets/drone.obj", "./assets/drone.png", vec3_new(1, 1, 1),
  //           vec3_new(2, -1, 16), vec3_new(0, 0, 0));
  // load_mesh("./assets/crab.obj", "./assets/crab.png", vec3_new(1, 1, 1),
  //           vec3_new(-2, 0, 8), vec3_new(0, 0, 0));
}

// this was just for fun
int x_translate = 0;

Draw_mode mode = WIRE_DOT;
Culling_mode culling = ON;

void process_input(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    SDL_GetMouseState(&mouse_x, &mouse_y);

    switch (event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        is_running = false;
      } else if (event.key.keysym.sym == SDLK_b) {
        // toggle window mode
        toggle_windowed_mode();
      } else if (event.key.keysym.sym == SDLK_0) {
        toggle_classic_mode();
        break;
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
      }

      // camera movement
      else if (event.key.keysym.sym == SDLK_w) {
        camera_go_forward(delta_time);
      } else if (event.key.keysym.sym == SDLK_s) {
        camera_go_back(delta_time);
      } else if (event.key.keysym.sym == SDLK_a) {
        camera_yaw_left(1.0, delta_time);
      } else if (event.key.keysym.sym == SDLK_d) {
        camera_yaw_right(1.0, delta_time);
      } else if (event.key.keysym.sym == SDLK_LSHIFT) {
        camera_go_up(delta_time);
      } else if (event.key.keysym.sym == SDLK_LCTRL) {
        camera_go_down(delta_time);
      } else if (event.key.keysym.sym == SDLK_UP) {
        camera_pitch_up(1.0, delta_time);
      } else if (event.key.keysym.sym == SDLK_DOWN) {
        camera_pitch_down(1.0, delta_time);
      } else if (event.key.keysym.sym == SDLK_p) {
        optimized_rendering = !optimized_rendering;
      }
      break;
    }
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

  for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++) {
    mesh_t *mesh = get_mesh(mesh_index);

    vec3_t up_direction = {0, 1, 0};
    vec3_t target = {0, 0, 1};

    // calculate yaw
    mat4_t camera_yaw_rotation = mat4_make_rotation(get_camera().yaw_angle, Y);
    vec4_t direction_yaw = mat4_mul_vec4(
        camera_yaw_rotation,
        (vec4_t){.x = target.x, .y = target.y, .z = target.z, .w = 0.0f});

    // calculate pitch
    mat4_t camera_pitch_rotation =
        mat4_make_rotation(get_camera().pitch_angle, X);
    vec4_t final_direction =
        mat4_mul_vec4(camera_pitch_rotation, (vec4_t){.x = direction_yaw.x,
                                                      .y = direction_yaw.y,
                                                      .z = direction_yaw.z,
                                                      .w = 0.0f});

    set_camera(direction_yaw, final_direction);

    target = vec3_add(get_camera().position, get_camera().direction);

    // create view matrix
    view_matrix = mat4_look_at(get_camera().position, target, up_direction);

    mat4_t scale_matrix =
        mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);

    mat4_t translation_matrix = mat4_make_translation(
        mesh->translation.x, mesh->translation.y, mesh->translation.z);

    mat4_t rotation_matrix_x = mat4_make_rotation(mesh->rotation.x, X);
    mat4_t rotation_matrix_y = mat4_make_rotation(mesh->rotation.y, Y);
    mat4_t rotation_matrix_z = mat4_make_rotation(mesh->rotation.z, Z);

    // Loop all triangle faces of our mesh
    int num_faces = array_length(mesh->mesh_faces);
    for (int i = 0; i < num_faces; i++) {
      face_t mesh_face = mesh->mesh_faces[i];

      vec3_t face_vertices[3];
      face_vertices[0] = mesh->vertices[mesh_face.a];
      face_vertices[1] = mesh->vertices[mesh_face.b];
      face_vertices[2] = mesh->vertices[mesh_face.c];

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

        // Calculate how aligned the camera ray is with the face normal (using
        // dot product)
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
          projected_points[j].x *= get_window_width() / 2.0f;
          projected_points[j].y *= get_window_height() / 2.0f;

          // Invert the y values to account for flipped screen y coordenate
          projected_points[j].y *= -1;

          // translate the projected points to the middle of the screen
          projected_points[j].x += (int)(get_window_width() / 2.0f);
          projected_points[j].y += (int)(get_window_height() / 2.0f);
        }

        vec3_t light_direction = (vec3_t){
            .x = 0.0f,
            .y = 0.0f,
            .z = 1.0f,
        };
        light_t light = init_light(light_direction);
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
            .color = transformed_triangle.color,
            .texture = mesh->texture};

        if (num_triangles_to_render >= MAX_TRIANGLES_PER_MESH) {
          continue;
        }

        // Save the projected triangle in the array of triangles to render
        triangles_to_render[num_triangles_to_render++] = projected_triangle;
      }
    }
  }
}

void render(void) {
  clear_color_buffer(0xFF000000);
  clear_z_buffer();

  for (int i = 0; i < MAX_TRIANGLES_PER_MESH; i++) {
    triangle_2d_t triangle = triangles_to_render[i];

    if (mode == TEXTURED || mode == TEXTURED_WIRE) {
      optimized_rendering
          ? triangle_fill_texture_optimized(triangle, triangle.color)
          : draw_textured_triangle(triangle, mode);
    } else {
      optimized_rendering ? triangle_fill_optimized(triangle, triangle.color)
                          : draw_triangle(triangle, triangle.color, mode);
    }

    triangles_to_render[i] = (triangle_2d_t){
        .color = 0x000000,
    };
  }
  render_color_buffer();
}

int main(void) {

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
