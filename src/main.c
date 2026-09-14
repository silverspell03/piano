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

/**
 * @brief Point d'entrée : initialise SDL, crée l'application, lance la
 *        boucle principale (poll events + update), puis nettoie.
 *
 * @param argc Nombre d'arguments.
 * @param argv argv[1] optionnel : fréquence initiale de l'oscillateur en
 *             Hz (440.0f par défaut si absent).
 *
 * @return 0 en cas de succès, -1 si create_app() échoue.
 *
 * @warning Le polling d'événements ici est minimal (seul SDL_EVENT_QUIT
 *          est traité) et n'appelle pas app_handle_event() : la logique
 *          de resize et de clic souris définie dans app.c n'est donc
 *          jamais exécutée.
 */
int main(int argc, char **argv) {
  float freq;
  if (argc == 1) {
    freq = 440.0f;
  } else {
    freq = atof(argv[1]);
  }
  srand(time(NULL)); // Seed de rand() avec le temps actuel

  // Init des divers composants essentiel de SDL3
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  printf("%f\n", freq);

  // Création de la structure App
  App *app = create_app(freq);
  if (!app) {
    return -1;
  }

  bool running = true;
  uint64_t last = SDL_GetTicks();

  while (running) {
    SDL_Event e;

    // la fonction renvoie l'état app->running
    running = app_handle_event(app);

    uint64_t now = SDL_GetTicks();
    float dt = (now - last) / 1000.0f;
    last = now;

    app_update(app, dt);
  }

  destroy_app(app);
  return 0;
}
