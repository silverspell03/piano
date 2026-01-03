#include "app.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "graphics.h"
#include "ringbuffer.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 48000.0
#define CHANNELS 1
#define FPS 60.0
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FPS)
#define VISUAL_SAMPLES (SAMPLE_RATE / 10.0) // show last 100 ms ~4800 samples
#define CHUNK_SAMPLES 1024
#define WIDTH 900
#define HEIGTH 600

App *create_app() {
  App *app = malloc(sizeof(*app));
  if (SDL_CreateWindowAndRenderer("Piano!", WIDTH, HEIGTH, SDL_WINDOW_RESIZABLE,
                                  &app->win, &app->ren)) {
    printf("%s\n", SDL_GetError());
    free(app);
    return NULL;
  }
  app->running = 1;
  app->uictx = create_ui(app->ren);

  SDL_AudioSpec spec = {SDL_AUDIO_F32, 1, 48000};
  app->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          &spec, NULL, NULL);

  SDL_ResumeAudioStreamDevice(app->stream);
  rb_init(app->vrb, 8192);

  return app;
}

void app_update(App *app) {
  // Reset du background en noir
  SDL_SetRenderDrawColor(app->ren, 0, 0, 0, 255);
  SDL_RenderClear(app->ren);

  SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
  size_t samples_to_draw = app->width;
  size_t start = (app->vrb->head >= samples_to_draw)
                     ? (app->vrb->head - samples_to_draw)
                     : (app->vrb->head + app->vrb->size - samples_to_draw);

  // Rendu des points de l'onde sonore
  SDL_FPoint *points = malloc(samples_to_draw * sizeof(SDL_FPoint));
  for (int x = 0; x < samples_to_draw; x++) {
    float r = app->vrb->buf[(start + x) & app->vrb->mask];
    int y = (int)(app->height * 0.5f - r * (app->height * 0.2f));
    points[x].x = (float)x;
    points[x].y = (float)y;
  }
  SDL_RenderLines(app->ren, points, samples_to_draw);
  draw_ui(app->uictx);
  SDL_RenderPresent(app->ren);
  GenerateNoise(app->stream, SAMPLES_PER_FRAME);
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
