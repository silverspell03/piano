#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "ui.h"
#include <SDL3/SDL.h>

typedef struct AppState AppState;

struct AppState {
  bool running;
  SDL_Event *e;
  UIContext *uictx;
  SDL_Window *win;
  SDL_Renderer *ren;
  int width;
  int height;
  SDL_AudioStream *stream;
};

int destroy_app(AppState *app);
AppState *create_app(SDL_Window *win, SDL_Renderer *ren,
                     SDL_AudioSpec *spec); // Creation du app contexte

void handle_input(AppState *app); // Fonction s'occupant de la gestion des
                                  // inputs
