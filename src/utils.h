#ifndef UTILS_H
#define UTILS_H
#include "display.h"

void draw_grid(const int h, const int w, const int size, const uint32_t color);
void draw_rect(const int x, const int y, const int width, const int height,
               const uint32_t color);
void draw_pixel(const int x, const int y, const uint32_t color);
#endif
