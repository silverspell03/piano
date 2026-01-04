#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include "ringbuffer.h"
#include "ui.h"
#include <SDL3/SDL.h>

typedef struct App {
  bool running;
  UIContext *uictx;
  SDL_Window *win;
  SDL_Renderer *ren;
  int width;
  int height;
  SDL_AudioStream *stream;
  void *a_buf;
  RingBuffer *vrb;
} App;

App *create_app(); // Creation du app contexte
// Fonction s'occupant de la gestion des Inputs
void app_handle_event(App *app);
void app_update(App *app);
void app_draw(App *app);
int destroy_app(App *app);
