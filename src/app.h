#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include "ringbuffer.h"
#include "ui.h"
#include <SDL3/SDL.h>

typedef struct App App;
typedef struct AppCfg AppCfg;

struct App {
  bool running;
  UIContext *uictx;
  SDL_Window *win;
  SDL_Renderer *ren;
  int width;
  int height;
  SDL_AudioStream *stream;
  RingBuffer *vrb;
};

struct AppCfg {
  char *title;
  int win_w;
  int win_h;
  SDL_AudioFormat a_format;
  int a_sample_rate;
  float a_freq;
};

App *create_app(); // Creation du app contexte
// Fonction s'occupant de la gestion des Inputs
void app_handle_event(App *app);
void app_update(App *app);
void app_draw(App *app);
int destroy_app(App *app);
