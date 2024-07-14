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

int window_height = 600;
int window_width = 800;

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

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  // Start x_start and x_end from the top vertex (x0,y0)
  float x_start = x0;
  float x_end = x0;

  // Loop all the scanlines from top to bottom
  for (int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);
    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  // Find the two slopes (two triangle legs)
  float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
  float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

  // Start x_start and x_end from the bottom vertex (x2,y2)
  float x_start = x2;
  float x_end = x2;

  // Loop all the scanlines from bottom to top
  for (int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);
    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

void draw_triangle(triangle_2d_t triangle, uint32_t color, Draw_mode draw_mode) {


  int x0 = triangle.points[0].x, y0 = triangle.points[0].y;
  int x1 = triangle.points[1].x, y1 = triangle.points[1].y;
  int x2 = triangle.points[2].x, y2 = triangle.points[2].y;

  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
  }
  if (y1 > y2) {
    swap(&y1, &y2, sizeof(int));
    swap(&x1, &x2, sizeof(int));
  }
  if (y0 > y1) {
    swap(&y0, &y1, sizeof(int));
    swap(&x0, &x1, sizeof(int));
  }

  if (draw_mode == WIRE || draw_mode == WIRE_DOT || draw_mode == SOLID_WIRE) {
    draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
              triangle.points[1].y, color - 0xFF);
    draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x,
              triangle.points[2].y, color - 0xFF);
    draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x,
              triangle.points[0].y, color - 0xFF);
    if(draw_mode == WIRE) return;
  }

  if(draw_mode == WIRE_DOT){
    // TODO:
    return;
  }

  // para evitar dividir por 0
  if (y1 == y2) {
    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
  } else if (y0 == y1) {
    // Draw flat-top triangle
    fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
  } else {
    // Calculate the new vertex (Mx,My) using triangle similarity
    int My = y1;
    int Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, Mx, My, x1, y1, color);

    // Draw flat-top triangle
    fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
  }
}
