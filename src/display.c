#include "display.h"
#include <math.h>
#include <stdlib.h>

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
  if(x < 0 || x >= window_width || y < 0 || y >= window_height) {
    return;
  }
  color_buffer[window_width * y + x] = color;
}

void draw_line(const int x0, const int y0, const int x1, const int y1){
  const int delta_x = (x1 - x0);
  const int delta_y = (y1 - y0);

  const int side_length = abs(delta_x);

  const float inc_x = delta_x / (float) side_length;
  const float inc_y = delta_y / (float) side_length;

  float current_x = x0;
  float current_y = y0;

  for(int i = 0; i < side_length; i++){
    draw_pixel(round(current_x), round(current_y), 0xFFFF0000);
    current_x += inc_x;
    current_y += inc_y;
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
      // Check if the current column should have a vertical line
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
