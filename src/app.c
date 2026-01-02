#include "app.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

AppState *create_app(SDL_Window *win, SDL_Renderer *ren, SDL_AudioSpec *spec) {
  AppState *app = malloc(sizeof(AppState));
  app->running = 1;
  app->win = win;
  app->ren = ren;
  app->uictx = create_ui(ren);

  if (!create_audio()) {
    printf("Error init audio stream\n");
    return NULL;
  }

  return app;
}

int destroy_app(AppState *app) {
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
