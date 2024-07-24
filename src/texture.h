#ifndef TEX_H
#define TEX_H

#include "upng.h"
#include <stdint.h>

typedef struct {
  float u;
  float v;
} tex2_t;

tex2_t tex2_clone(tex2_t* t);

#endif
