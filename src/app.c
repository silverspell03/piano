#include "app.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include "graphics.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 48000.0
#define CHANNELS 1
#define FPS 60.0
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FPS)

AppCtx *create_app(AppCfg *cfg) {
  AppCtx *app = malloc(sizeof(AppCtx));
  SDL_Window *win;
  SDL_Renderer *ren;
  if (SDL_CreateWindowAndRenderer(cfg->title, cfg->win_w, cfg->win_h,
                                  SDL_WINDOW_RESIZABLE, &win, &ren)) {
    printf("%s\n", SDL_GetError());
    free(app);
    return NULL;
  }
  app->running = 1;
  app->uictx = create_ui(ren);

  AudioConfig *audio_cfg = {cfg->a_freq, cfg->a_sample_rate, cfg->a_format, 1,
                            SAMPLES_PER_FRAME};
  app->audio = create_audio(&audio_cfg);
  if (!create_audio(&cfg->audio)) {
    printf("Error init audio stream\n");
    return NULL;
  }

  return app;
}

int destroy_app(AppCtx *app) {
  if (!app) {
    return -1;
  }
  ui_destroy(app->uictx);
  free(app);
  return 0;
}

void handle_input(AppState *app) {
  SDL_Event e;

  // On poll les events pour fermer le programme
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      float x, y;
    case SDL_EVENT_QUIT:
      app->running = false;
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      SDL_GetWindowSize(app->win, &app->width, &app->height);
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (SDL_GetRelativeMouseState(&x, &y) != SDL_BUTTON_LEFT) {
        break;
      }
    }
    // handle input to change freq, etc.
  }
}
