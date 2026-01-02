#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "app.h"
#include "graphics.h"
#include "ringbuffer.h"
#include <SDL3/SDL.h>
#include <bits/time.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLE_RATE 48000.0
#define CHANNELS 1
#define FPS 60.0
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FPS)
#define VISUAL_SAMPLES (SAMPLE_RATE / 10) // show last 100 ms ~4800 samples
#define CHUNK_SAMPLES 1024

/* This function runs once at startup. */
int main(void) {
  srand(time(NULL)); // Seed de rand() avec le temps actuel
  // Init des divers composants essentiel de SDL3
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  AppCfg cfg = {
      .title = "Piano!",
      .win_w = 900,
      .win_h = 600,
      .a_format = SDL_AUDIO_F32,
      .a_sample_rate = 48000,
      .a_freq = 440.0f,
  };
  AppCtx *app = create_app(&cfg);
  bool running = true;
  uint64_t last = SDL_GetTicks();

  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_EVENT_QUIT)
        running = false;

      app_handle_event(app, &ev);
    }

    uint64_t now = SDL_GetTicks();
    float dt = (now - last) / 1000.0f;
    last = now;

    app_update(app, dt);
    app_draw(app);
  }

  app_destroy(app);
  SDL_Quit();

  show_ui(ui);


  // Main loop
  while (app->running) {
  }

  rb_free(&vis);
  free(chunk);
  SDL_DestroyAudioStream(stream);
  destroy_app(app);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
