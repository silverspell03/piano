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

/* This function runs once at startup. */
int main(void) {
  srand(time(NULL)); // Seed de rand() avec le temps actuel
  // Init des divers composants essentiel de SDL3
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  App *app = create_app();
  bool running = true;
  uint64_t last = SDL_GetTicks();
  SDL_Event e;

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT)
        running = false;
    }

    uint64_t now = SDL_GetTicks();
    float dt = (now - last) / 1000.0f;
    last = now;

    app_update(app);
  }

  destroy_app(app);
  return 0;
}
