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
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_CreateWindowAndRenderer("Piano!", 900, 600, SDL_WINDOW_RESIZABLE, &win,
                              &ren);
  AppState *app = create_app(win, ren);
  UIContext *ui = app->uictx;

  show_ui(ui);

  // Spécification des specs audio attendues, Float 32 bits
  SDL_AudioSpec spec;
  spec.channels = CHANNELS;
  spec.format = SDL_AUDIO_F32;
  spec.freq = SAMPLE_RATE;

  float *chunk = malloc(CHUNK_SAMPLES * sizeof(float));
  float *out = malloc(VISUAL_SAMPLES * sizeof(float));
  RingBuffer vis;
  rb_init(&vis, 8192);

  Osc osc;
  osc.phase = 0.0f;
  osc.freq = 110.0f;

  // Main loop
  while (app->running) {
    handle_input(app);
    float phase_inc = M_PI * 2 * osc.freq / SAMPLE_RATE;

    for (int i = 0; i < CHUNK_SAMPLES; i++) {
      float r = sin(osc.phase);
      // if (r > 0) {
      //   r = 1.0;
      // } else if (r < 0) {
      //   r = -1.0;
      // } else if (r == 0) {
      //   r = 0.0;
      // }
      chunk[i] = r;
      osc.phase += phase_inc;
      if (osc.phase >= M_PI * 2) {
        osc.phase -= M_PI * 2;
      }
    }
    // Génération du bruit dans le buffer chunk
    // GenerateNoise(chunk, CHUNK_SAMPLES);
    // On pousse les données dans le stream audio puis copie vers le ringbuffer
    // visuel
    SDL_PutAudioStreamData(stream, chunk, CHUNK_SAMPLES * sizeof(sample_t));
    rb_write_block(&vis, chunk, CHUNK_SAMPLES);

    // Reset du background en noir
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    size_t samples_to_draw = w;
    size_t start = (vis.head >= samples_to_draw)
                       ? (vis.head - samples_to_draw)
                       : (vis.head + vis.size - samples_to_draw);

    // Rendu des points de l'onde sonore
    SDL_FPoint *points = malloc(samples_to_draw * sizeof(SDL_FPoint));
    for (int x = 0; x < samples_to_draw; x++) {
      float r = vis.buf[(start + x) & vis.mask];
      int y = (int)(h * 0.5f - r * (h * 0.2f));
      points[x].x = (float)x;
      points[x].y = (float)y;
    }
    SDL_RenderLines(ren, points, samples_to_draw);
    draw_ui(ui);
    SDL_RenderPresent(ren);
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
