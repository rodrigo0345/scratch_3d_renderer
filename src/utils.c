#include "utils.h"

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
        color_buffer[window_width * y + x] = color;
        fill_vborder--;
      } else if ((x && (x % w == 0)) || continuous_hline) {
        color_buffer[window_width * y + x] = color;
        fill_vborder = size - 1;
      }
    }
  }
}

void draw_rect(const int x, const int y, const int width, const int height,
               const uint32_t color) {
  for (int y_it = y; y_it < y + height; y_it++) {
    for (int x_it = x; x_it < x + width; x_it++) {
      color_buffer[y_it * window_width + x_it] = color;
    }
  }
}
