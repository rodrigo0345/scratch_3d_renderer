#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// the color buffer needs to be non implementation specific as each color
// uses 32bits/4bytes in memory
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_height = 600;
int window_width = 800;

bool initialize_window(void);
void destroy_window(void);

void clear_color_buffer(uint32_t color);
void render_color_buffer(void);

#endif // !DISPLAY_H
#define DISPLAY_H
