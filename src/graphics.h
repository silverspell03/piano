#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "app.h"
#include <SDL3/SDL.h>

// Dessiner un cercle a partir de la méthode Bresenham
void draw_circle(SDL_Renderer *ren, float cx, float cy, float r);
void draw_data(App *app, int samples, RingBuffer *vrb);

#endif
