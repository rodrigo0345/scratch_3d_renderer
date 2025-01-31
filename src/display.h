#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  WIRE_DOT,
  WIRE,
  SOLID,
  SOLID_WIRE,
  TEXTURED,
  TEXTURED_WIRE
} Draw_mode;

typedef enum { ON, OFF } Culling_mode;

typedef uint32_t color_t;

bool initialize_window(void);
void destroy_window(void);

int get_window_width(void);
int get_window_height(void);

void clear_color_buffer(uint32_t color);
void clear_z_buffer();
void render_color_buffer(void);

void draw_grid(const int h, const int w, const int size, const color_t color);
void draw_rect(const int x, const int y, const int width, const int height,
               const color_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
void draw_line(const int x0, const int y0, const int x1, const int y1,
               color_t color);
void swap(void *a, void *b, size_t size);
SDL_Renderer *get_renderer(void);
uint32_t *get_color_buffer(void);
void toggle_classic_mode(void);
void toggle_windowed_mode(void);

void clear_z_buffer(void);
float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);

#endif // !DISPLAY_H
