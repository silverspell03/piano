#include "ui.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"

struct UIContext {
  SDL_Window *win;
  SDL_Renderer *ren;
  int width, heigth;
  Widget *widgets;
  int mouse_x, mouse_y;
  int press_x, press_y;
  int release_x, release_y;
  bool mouse_down;
  bool mouse_released;
};

UIContext *create_ui()
{
  UIContext *ui = malloc(sizeof(UIContext));
  SDL_CreateWindowAndRenderer("Piano!", ui->width, ui->heigth,
                              SDL_WINDOW_RESIZABLE, &ui->win, &ui->ren);
  SDL_GetWindowSize(ui->win, &ui->width, &ui->heigth);
  ui->heigth = 600;
  ui->width = 900;
  SDL_SetRenderVSync(ui->ren, 1);
  return ui;
}

void free_ui(UIContext *ui) {
  SDL_DestroyRenderer(ui->ren);
  SDL_DestroyWindow(ui->win);
  SDL_Quit();
  free(ui);
}

void draw_ui(UIContext *ui) {
  int width = ui->width;
  int heigth = ui->heigth;
  SDL_Renderer *ren = ui->ren;
  SDL_SetRenderDrawColor(ren, 0, 200, 200, 255);

  SDL_FRect rect;
  rect.y = heigth - (heigth / 4.0);
  rect.x = width - (width / 4.0);
  rect.w = width / 4.0;
  rect.h = heigth / 4.0;

  SDL_FPoint p1 = {rect.x, rect.y + (rect.h / 2.0f)};
  SDL_FPoint p2 = {rect.x + rect.w, rect.y + (rect.h / 2.0f)};
  float cx = width + (p1.x - p2.x) / 2.0;
  float cy = p1.y;

  SDL_RenderLine(ren, p1.x, p1.y, p2.x, p2.y);
  printf("cx = %f, cy = %f\n", cx, cy);
  draw_circle(ren, cx, cy, 50);
}
