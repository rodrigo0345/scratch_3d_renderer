#include "display.h"
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// the color buffer needs to be non implementation specific as each color
// uses 32bits/4bytes in memory
static uint32_t *color_buffer = NULL;
static SDL_Texture *color_buffer_texture = NULL;
static float *z_buffer = NULL;

static int window_height;
static int window_width;

static bool classic_mode = false;

int get_window_width(void) { return window_width; }
int get_window_height(void) { return window_height; }
SDL_Renderer *get_renderer(void) { return renderer; }

uint32_t *get_color_buffer(void) { return color_buffer; }

void toggle_classic_mode(void) {
  classic_mode = classic_mode ? false : true;
  initialize_window();
}

static int window_mode = SDL_WINDOW_FULLSCREEN;

void toggle_windowed_mode(void) {
  window_mode =
      window_mode == SDL_WINDOW_FULLSCREEN ? 0 : SDL_WINDOW_FULLSCREEN;
  initialize_window();
}

void clear_z_buffer(void) {
    for (int i = 0; i < window_width * window_height; i++) {
        z_buffer[i] = 1.0;
    }
}

float get_zbuffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return z_buffer[(window_width * y) + x];
}

void update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[(window_width * y) + x] = value;
}

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

  int fullscreen_width = display_mode.w;
  int fullscreen_height = display_mode.h;

  if (window_mode == 0) {
    fullscreen_height = 600;
    fullscreen_width = 800;
  }

  if (!classic_mode) {
    window_width = fullscreen_width;
    window_height = fullscreen_height;

  } else {
    window_width = fullscreen_width / 3;
    window_height = fullscreen_height / 3;
  }

  if (window)
    SDL_DestroyWindow(window);

  if (window_mode == SDL_WINDOW_FULLSCREEN) {
    window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, fullscreen_width,
                              fullscreen_height, window_mode);
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  } else {
    window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, fullscreen_width,
                              fullscreen_height, window_mode);
  }

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
  //
  if (color_buffer != NULL)
    free(color_buffer);
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_height * window_width);

  if (z_buffer != NULL)
    free(z_buffer);
  z_buffer = (float *)malloc(sizeof(float) * fullscreen_height * fullscreen_width);

  if (!color_buffer) {
    fprintf(stderr, "Error initializing frame buffer");
    return false;
  }

  ////////////////////////////////////////////////////////////////
  // buffer texture is going to be responsible for
  // translating our color_buffer to SDL
  ////////////////////////////////////////////////////////////////
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  return true;
}

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < window_height * window_width; y++) {
    color_buffer[y] = color;
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)window_width * sizeof(uint32_t));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
  SDL_RenderPresent(get_renderer());
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
