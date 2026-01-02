#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include "ui.h"
#include <SDL3/SDL.h>

typedef struct AppCtx AppCtx;
typedef struct AppCfg AppCfg;

struct AppCtx {
  bool running;
  SDL_Event *e;
  UIContext *uictx;
  SDL_Window *win;
  SDL_Renderer *ren;
  int width;
  int height;
  AudioCtx *a_ctx;
};

struct AppCfg {
  char *title;
  int win_w;
  int win_h;
  SDL_AudioFormat a_format;
  int a_sample_rate;
  float a_freq;
};

AppCtx *create_app(AppCfg *cfg); // Creation du app contexte

void handle_event(AppCtx *app); // Fonction s'occupant de la gestion des Inputs
int destroy_app(AppCtx *app);
