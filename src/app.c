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
#define CHUNK_SAMPLES 1024
#define WIDTH 900
#define HEIGHT 600

App *create_app(float freq) {
  App *app = calloc(1, sizeof(*app));

  if (!SDL_CreateWindowAndRenderer("Piano!", WIDTH, HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &app->win,
                                   &app->ren)) {
    printf("%s\n", SDL_GetError());
    free(app);
    return NULL;
  }
  app->running = 1;

  SDL_SetRenderVSync(app->ren, 1);

  SDL_AudioSpec spec = {SDL_AUDIO_F32, 1, 48000};
  app->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          &spec, NULL, NULL);
  SDL_ResumeAudioStreamDevice(app->stream);

  Osc osc;
  osc.freq = freq;
  osc.phase = 0.0f;

  app->osc = osc;

  // AVANT : vrb est un pointeur non initialisé (garbage), rb_init() écrit
  // à travers une adresse invalide -> comportement indéfini, corruption
  // mémoire potentielle.
  // RingBuffer *vrb;
  // if (rb_init(vrb, 8192) != 0) {
  //   printf("rb init issues\n");
  //   return NULL;
  // }
  // app->vrb = vrb;

  // APRES : on alloue réellement la struct avant de la passer à rb_init.
  RingBuffer *vrb = malloc(sizeof(RingBuffer));
  if (!vrb || rb_init(vrb, 8192) != 0) {
    printf("rb init issues\n");
    free(vrb);
    return NULL;
  }
  app->vrb = vrb;

  return app;
}

void app_update(App *app, float dt) {
  // AVANT : dt vient directement du delta-temps de la boucle principale.
  // En cas de freeze (resize, breakpoint, lag système), dt peut être grand
  // -> "samples" explose -> VLA énorme sur la pile -> stack overflow possible.
  // int samples = dt * SAMPLE_RATE;

  // APRES : on clampe dt à une valeur raisonnable (ex: 50ms max, ~2400
  // échantillons à 48kHz), bien en dessous de la taille du ring buffer (8192).
  if (dt > 0.05f)
    dt = 0.05f;
  int samples = dt * SAMPLE_RATE;

  SDL_SetRenderDrawColor(app->ren, 0, 0, 0, 255);
  SDL_RenderClear(app->ren);
  float phase_inc = M_PI * 2 * app->osc.freq / (float)SAMPLE_RATE;
  float data[samples];

  for (int i = 0; i < samples; ++i) {
    float r = sin(app->osc.phase) * 0.2f;
    data[i] = r;
    app->osc.phase += phase_inc;
    if (app->osc.phase >= (2 * M_PI)) {
      app->osc.phase -= 2 * M_PI;
    }
  }
  SDL_PutAudioStreamData(app->stream, data, samples * sizeof(sample_t));
  RingBuffer *vrb = app->vrb;
  rb_write_block(vrb, data, samples);
  draw_data(app, samples, vrb);
}

int destroy_app(App *app) {
  if (!app) {
    return -1;
  }
  rb_free(app->vrb);
  ui_destroy(app->uictx);
  free(app);
  return 0;
}

bool app_handle_event(App *app) {
  SDL_Event e;

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
      // clic gauche uniquement
      if (!(SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK)) {
        break;
      }
      // On mappe x (0..width) sur une plage de fréquence, ex: 100..1000 Hz
      if (app->width > 0) {
        float t = x / (float)app->width;     // 0.0 -> 1.0
        app->osc.freq = 100.0f + t * 900.0f; // 100..1000 Hz
      }
      break;
    }
  }
  return app->running;
}
