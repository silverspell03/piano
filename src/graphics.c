#include "SDL3/SDL_render.h"
#include "ringbuffer.h"
#include <SDL3/SDL.h>
#include <app.h>
#include <stdio.h>

// Dessiner un cercle a partir de la méthode Bresenham
static void circle_bres(SDL_Renderer *ren, float cx, float cy, float x,
                        float y) {
  SDL_RenderPoint(ren, cx + x, cy + y);
  SDL_RenderPoint(ren, cx - x, cy + y);
  SDL_RenderPoint(ren, cx + x, cy - y);
  SDL_RenderPoint(ren, cx - x, cy - y);
  SDL_RenderPoint(ren, cx + y, cy + x);
  SDL_RenderPoint(ren, cx - y, cy + x);
  SDL_RenderPoint(ren, cx + y, cy - x);
  SDL_RenderPoint(ren, cx - y, cy - x);
}

void draw_circle(SDL_Renderer *ren, float cx, float cy, float r) {
  float x = 0;
  float y = r;
  float d = 3 - 2 * r;
  circle_bres(ren, cx, cy, x, y);
  while (x <= y) {
    SDL_RenderPoint(ren, cx + x, cy + y);
    if (d < 0) {
      d = d + (4 * x) + 6;
    } else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
    circle_bres(ren, cx, cy, x, y);
  }
}

void draw_data(App *app, int samples, RingBuffer *vrb) {

  int width = app->width;
  int height = app->height;

  // Allocation d'un buffer de n samples
  sample_t out_buf[width];

  // Lecture des dernieres samples du ringbuffer afin de les noter dans
  // out_buf
  rb_read_block(vrb, out_buf, width);

  SDL_FPoint points[width];
  for (int i = 0; i < width; ++i) {
    points[i].x = (float)i;

    // Eviter les overflow
    float r = out_buf[i];

    // Il me semble que r est normalisé entre -1 et 1
    r = (r + 1.0f) / 2.0f * height;
    points[i].y = r;
  }

  SDL_SetRenderDrawColor(app->ren, 255, 50, 50, SDL_ALPHA_OPAQUE);
  SDL_RenderLines(app->ren, points, width);
  SDL_RenderPresent(app->ren);
}
