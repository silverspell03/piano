#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <bits/time.h>
#include <ctype.h>
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

typedef float sample_t;

typedef struct {
  sample_t *buf;
  size_t size; // power-of-two recommended
  size_t head; // write index (next write)
  size_t mask; // size - 1
} RingBuffer;

typedef struct {
  float phase;
  float freq;
} Osc;

typedef struct {
  SDL_Window *win;
  SDL_Renderer *ren;
  int width, heigth;
} UIContext;

static int rb_init(RingBuffer *rb, size_t size_pow2) {
  rb->size = size_pow2;
  rb->buf = malloc(rb->size * sizeof(sample_t));
  rb->head = 0;
  rb->mask = rb->size - 1;
  return 0;
}

static int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n) {
  for (int i = 0; i < n; i++) {
    rb->buf[(i + rb->head) & rb->mask] = in[i];
  }
  rb->head += n;
  return 0;
}

static int rb_read_block(const RingBuffer *rb, sample_t *out, size_t n) {
  for (int i = 0; i < n; i++) {
    out[i] = rb->buf[i - rb->head & rb->mask];
  }
  return 0;
}

static int rb_free(RingBuffer *rb) {
  free(rb->buf);
  rb->buf = NULL;
  return 0;
}

static void GenerateNoise(void *buf, int samples) {
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;

  if (!buf)
    return;

  float *data = buf;
  for (int i = 0; i < samples; i++) {
    float r = (float)rand() / (float)RAND_MAX;
    r = (r * 2.0 - 1.0) * 0.5; /* 0.2 = volume */
    data[i] = r;
  }
}

static void draw_circle(SDL_Renderer *ren, float cx, float cy, float x,
                        float y);

static void circle_bres(SDL_Renderer *ren, float cx, float cy, float r) {
  float x = 0;
  float y = r;
  float d = 3 - 2 * r;
  draw_circle(ren, cx, cy, x, y);
  while (x <= y) {
    SDL_RenderPoint(ren, cx + x, cy + y);
    if (d < 0) {
      d = d + (4 * x) + 6;
    } else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
    draw_circle(ren, cx, cy, x, y);
  }
}

// Dessiner un cercle a partir de la méthode Bresenham
static void draw_circle(SDL_Renderer *ren, float cx, float cy, float x,
                        float y) {
  SDL_RenderPoint(ren, cx + x, cy + y);
  SDL_RenderPoint(ren, cx - x, cy + y);
  SDL_RenderPoint(ren, cx + x, cy - y);
  SDL_RenderPoint(ren, cx - x, cy - y);
  SDL_RenderPoint(ren, cx + y, cy + x);
  SDL_RenderPoint(ren, cx - y, cy + x);
  SDL_RenderPoint(ren, cx + y, cy - x);
  SDL_RenderPoint(ren, cx - y, cy - x);
}

static void init_ui(UIContext *ui) {
  ui->heigth = 600;
  ui->width = 900;
  SDL_CreateWindowAndRenderer("Piano!", ui->width, ui->heigth,
                              SDL_WINDOW_RESIZABLE, &ui->win, &ui->ren);
  SDL_SetRenderVSync(ui->ren, 1);
}

static void free_ui(UIContext *ui) {
  SDL_DestroyRenderer(ui->ren);
  SDL_DestroyWindow(ui->win);
  SDL_Quit();
}

static void draw_ui(UIContext *ui) {
  int width = ui->width;
  int heigth = ui->heigth;
  SDL_Renderer *ren = ui->ren;
  SDL_SetRenderDrawColor(ren, 0, 200, 200, 255);

  SDL_FRect rect;
  rect.y = heigth - (heigth / 4.0);
  rect.x = width - (width / 4.0);
  rect.w = width / 4.0;
  rect.h = heigth / 4.0;

  SDL_FPoint p1 = {rect.x, rect.y + (rect.h / 2.0f)};
  SDL_FPoint p2 = {rect.x + rect.w, rect.y + (rect.h / 2.0f)};
  float cx = width + (p1.x - p2.x) / 2.0;
  float cy = p1.y;

  SDL_RenderLine(ren, p1.x, p1.y, p2.x, p2.y);
  printf("cx = %f, cy = %f\n", cx, cy);
  circle_bres(ren, cx, cy, 50);
}

/* This function runs once at startup. */
int main(void) {
  srand(time(NULL)); // Seed de rand() avec le temps actuel
  // Init des divers composants essentiel de SDL3
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  UIContext ui;
  init_ui(&ui);
  SDL_Renderer *ren = ui.ren;
  SDL_Window *win = ui.win;
  // Activation Vsync pour limiter le framerate

  // Spécification des specs audio attendues, Float 32 bits
  SDL_AudioSpec desired;
  desired.channels = CHANNELS;
  desired.format = SDL_AUDIO_F32;
  desired.freq = SAMPLE_RATE;

  // Ouverture du stream audio lié au device par défaut
  SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, NULL, NULL);
  SDL_ResumeAudioStreamDevice(stream);
  SDL_ShowWindow(win);

  bool running = 1;
  float *chunk = malloc(CHUNK_SAMPLES * sizeof(float));
  float *out = malloc(VISUAL_SAMPLES * sizeof(float));
  RingBuffer vis;
  rb_init(&vis, 8192);

  Osc osc;
  osc.phase = 0.0f;
  osc.freq = 110.0f;

  // Main loop
  while (running) {
    SDL_Event e;
    // On poll les events pour fermer le programme
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        float x, y;
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSize(win, &ui.width, &ui.heigth);
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (SDL_GetRelativeMouseState(&x, &y) != SDL_BUTTON_LEFT) {
          break;
        }
      }
      // handle input to change freq, etc.
    }
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
    draw_ui(&ui);
    SDL_RenderPresent(ren);
  }

  rb_free(&vis);
  free(chunk);
  SDL_DestroyAudioStream(stream);
  free_ui(&ui);
  return 0;
}
