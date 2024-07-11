#include "display.h"
#include "triangle.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// the color buffer needs to be non implementation specific as each color
// uses 32bits/4bytes in memory
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_height = 1080;
int window_width = 1920;

bool initialize_window(void) {
  // creating a window
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  // use SDL to query the screen height and width
  SDL_DisplayMode display_mode;

  // get the main window (0) information
  SDL_GetCurrentDisplayMode(0, &display_mode);

  // window_width = display_mode.w;
  // window_height = display_mode.h;

  window =
      SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);

  if (!window) {
    fprintf(stderr, "Error creating SDL Window");
    return false;
  }

  // -1 tells sdl to get the default graphics
  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) {
    fprintf(stderr, "Error creating SDL Renderer");
    return false;
  }
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      color_buffer[(window_width * y) + x] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)window_width * sizeof(uint32_t));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void destroy_window(void) {
  free(window);
  SDL_DestroyTexture(color_buffer_texture);
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}

void draw_pixel(const int x, const int y, const uint32_t color) {
  if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
    return;
  }
  color_buffer[window_width * y + x] = color;
}

void draw_line(const int x0, const int y0, const int x1, const int y1,
               const uint32_t color) {
  int delta_x = (x1 - x0);
  int delta_y = (y1 - y0);

  int longest_side_length =
      (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

  float x_inc = delta_x / (float)longest_side_length;
  float y_inc = delta_y / (float)longest_side_length;

  float current_x = x0;
  float current_y = y0;
  for (int i = 0; i <= longest_side_length; i++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
}

void draw_grid(const int h, const int w, const int size, const uint32_t color) {

  // Iterate over each pixel in the window
  int fill_hborder = 0;
  int fill_vborder = 0;
  for (int y = 0; y < window_height; y++) {
    bool continuous_hline = false;

    if (fill_hborder) {
      continuous_hline = true;
      fill_hborder--;
    } else if (y && (y % h == 0)) {
      continuous_hline = true;
      fill_hborder = size - 1;
    }

    // Iterate over each column in the current row
    for (int x = 0; x < window_width; x++) {

      if (fill_vborder) {
        draw_pixel(x, y, color);
        fill_vborder--;
      } else if ((x && (x % w == 0)) || continuous_hline) {
        draw_pixel(x, y, color);
        fill_vborder = size - 1;
      }
    }
  }
}

void draw_rect(const int x, const int y, const int width, const int height,
               const uint32_t color) {
  for (int y_it = y; y_it < y + height; y_it++) {
    for (int x_it = x; x_it < x + width; x_it++) {
      draw_pixel(x_it, y_it, color);
    }
  }
}

void swap(void *a, void *b, size_t size) {
  void *tmp = malloc(size);
  if (tmp != NULL) {
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
  }
}

void fill_flat_bottom_triangle(triangle_t t) {}

void fill_flat_top_triangle(triangle_t t) {}

void draw_triangle(triangle_t triangle, bool wireframe, uint32_t color) {
  if (wireframe) {
    draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
              triangle.points[1].y, color);
    draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x,
              triangle.points[2].y, color);
    draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x,
              triangle.points[0].y, color);
    return;
  }

  // order all the points
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (triangle.points[j].y > triangle.points[i].y) {
        swap(&triangle.points[j], &triangle.points[i], sizeof(vec2_t));
      }
    }
  }

  const vec2_t *middle_point = &triangle.points[1];
  const vec2_t *top_point = &triangle.points[2];
  const vec2_t *bottom_point = &triangle.points[0];

  // clunky debug
  // printf("x0: %f, y0: %f\n", top_point->x, top_point->y);
  // printf("x1: %f, y1: %f\n", middle_point->x, middle_point->y);
  // printf("x2: %f, y2: %f\n", bottom_point->x, bottom_point->y);

  // chegamos a esta formula pela similaridade de triangulos
  const float Mx =
      ((middle_point->y - top_point->y) / (float)(bottom_point->y - top_point->y)) *
          (bottom_point->x - top_point->x) +
      top_point->x;

  // draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
  //           triangle.points[1].y, color);
  // draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x,
  //           triangle.points[2].y, color);
  // draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x,
  //           triangle.points[0].y, color);

  draw_line(Mx, middle_point->y, Mx, middle_point->y, 0xFF00FF00);

  fill_flat_bottom_triangle(
      (triangle_t){.points = {
                       (vec2_t){.x = Mx, .y = middle_point->y},
                       *middle_point,
                       *bottom_point,
                   }});

  fill_flat_top_triangle(
      (triangle_t){.points = {
                       (vec2_t){.x = Mx, .y = middle_point->y},
                       *middle_point,
                       *top_point,
                   }});
}
