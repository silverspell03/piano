#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
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
  // Allocate local buffer for audio samples
  sample_t out_buf[samples];

  rb_read_block(vrb, out_buf, samples);

  SDL_FPoint points[samples];
  for (int i = 0; i < samples; ++i) {
    points[i].x = (float)i;
    float r = out_buf[i];
    r = (r + 1.0f) / 2.0f * 400;
    points[i].y = r;
  }

  SDL_SetRenderDrawColor(app->ren, 255, 50, 50, SDL_ALPHA_OPAQUE);
  SDL_RenderLines(app->ren, points, samples);
  SDL_RenderPresent(app->ren);
}
