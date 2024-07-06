#include "display.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool is_running = true;

void setup(void) {
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_height * window_width);

  if (!color_buffer) {
    fprintf(stderr, "Error initializing frame buffer");
    return;
  }

  ////////////////////////////////////////////////////////////////
  // buffer texture is going to be responsible for
  // translating our color_buffer to SDL
  ////////////////////////////////////////////////////////////////
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      is_running = false;
    }
    break;
  }
}

void update(void) {
  draw_rect(400, 200, 300, 200, 0xFFFF0000);
  draw_grid(100, 100, 4, 0xFFFFFFFF);
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  render_color_buffer();

  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

int main(void) {

  is_running = initialize_window();

  setup();
  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();

  return 0;
}
