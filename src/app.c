#include "app.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "graphics.h"
#include "ringbuffer.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 48000
#define CHANNELS 1
#define FPS 60
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FPS)
#define VISUAL_SAMPLES (SAMPLE_RATE / 10.0) // show last 100 ms ~4800 samples
#define CHUNK_SAMPLES 1024
#define WIDTH 900
#define HEIGTH 600

App *create_app() {
  App *app = calloc(1, sizeof(*app));

  if (!SDL_CreateWindowAndRenderer("Piano!", WIDTH, HEIGTH,
                                   SDL_WINDOW_RESIZABLE, &app->win,
                                   &app->ren)) {
    printf("%s\n", SDL_GetError());
    free(app);
    return NULL;
  }
  // Reset du background en noir
  SDL_SetRenderDrawColor(app->ren, 0, 0, 0, 255);
  SDL_RenderClear(app->ren);

  app->running = 1;

  SDL_SetRenderVSync(app->ren, 1);

  SDL_AudioSpec spec = {SDL_AUDIO_F32, 1, 48000};
  app->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          &spec, NULL, NULL);
  SDL_ResumeAudioStreamDevice(app->stream);

  Osc osc;
  osc.freq = 4440.0f;
  osc.phase = 0.0f;

  app->osc = osc;
  return app;
}

void app_update(App *app, float dt) {
  float phase_inc = M_PI * 2 * app->osc.freq / (float)SAMPLE_RATE;
  int samples = dt * SAMPLE_RATE;
  float data[samples];

  for (int i = 0; i < samples; ++i)
  {
    float r = sin(app->osc.phase) * 0.2f;
    data[i] = r;
    app->osc.phase += phase_inc;
    if (app->osc.phase >= (2*M_PI)) {app->osc.phase -= 2*M_PI;}
  }
  SDL_PutAudioStreamData(app->stream, data, samples * sizeof(sample_t));
}

int destroy_app(App *app) {
  if (!app) {
    return -1;
  }
  ui_destroy(app->uictx);
  free(app);
  return 0;
}

void app_handle_event(App *app) {
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
