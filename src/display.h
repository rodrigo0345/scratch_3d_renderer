#ifndef DISPLAY_H
#define DISPLAY_H

#include "triangle.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 50 

// how much time each frame needs to take
// we then need to tell the cpu to wait this time
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;

// the color buffer needs to be non implementation specific as each color
// uses 32bits/4bytes in memory
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;

extern int window_height;
extern int window_width;

bool initialize_window(void);
void destroy_window(void);

void clear_color_buffer(uint32_t color);
void render_color_buffer(void);

void draw_grid(const int h, const int w, const int size, const uint32_t color);
void draw_rect(const int x, const int y, const int width, const int height,
               const uint32_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
void draw_line(const int x0, const int y0, const int x1, const int y1, uint32_t color);
void draw_triangle(
  triangle_t triangle, bool wireframe, uint32_t color
);

#endif // !DISPLAY_H
