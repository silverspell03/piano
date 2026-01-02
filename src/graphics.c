#include "graphics.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <stdio.h>

int init_graphics(AppCtx *app, AppCfg *cfg) {
  if (!SDL_CreateWindowAndRenderer(cfg->title, cfg->win_w, cfg->win_h,
                                   SDL_WINDOW_RESIZABLE, &app->win,
                                   &app->ren)) {
    printf("%s\n", SDL_GetError());
    return -1;
  }
  return 0;
}
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
