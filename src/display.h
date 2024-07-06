#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

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

#endif // !DISPLAY_H
