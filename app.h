#include <SDL3/SDL.h>
#include "ui.h"

typedef struct AppState AppState;

struct AppState{
  bool running;
  SDL_Event *e;
  UIContext *uictx;
};

void handle_input(AppState *app);
