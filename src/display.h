#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 60 

typedef enum {
  WIRE_DOT,
  WIRE,
  SOLID,
  SOLID_WIRE,
  TEXTURED,
  TEXTURED_WIRE
} Draw_mode;

typedef enum {
  ON,
  OFF
} Culling_mode;

typedef uint32_t color_t;

// how much time each frame needs to take
// we then need to tell the cpu to wait this time
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;

// the color buffer needs to be non implementation specific as each color
// uses 32bits/4bytes in memory
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;
extern float* z_buffer;

extern int window_height;
extern int window_width;

bool initialize_window(void);
void destroy_window(void);

void clear_color_buffer(uint32_t color);
void clear_z_buffer();
void render_color_buffer(void);

void draw_grid(const int h, const int w, const int size, const color_t color);
void draw_rect(const int x, const int y, const int width, const int height,
               const color_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
void draw_line(const int x0, const int y0, const int x1, const int y1, color_t color);
void swap(void *a, void *b, size_t size);

#endif // !DISPLAY_H
