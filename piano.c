#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
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

static int rb_init(RingBuffer *rb, size_t size_pow2) {
  rb->size = size_pow2;
  rb->buf = malloc(rb->size * sizeof(&rb));
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

/* This function runs once at startup. */
int main(void) {
  srand(time(NULL)); // seed with current time
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_Window *win = NULL;
  SDL_Renderer *ren = NULL;
  SDL_CreateWindowAndRenderer("Piano!", 900, 600, 0, &win, &ren);

  SDL_AudioSpec desired;
  desired.channels = CHANNELS;
  desired.format = SDL_AUDIO_F32;
  desired.freq = SAMPLE_RATE;

  SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, NULL, NULL);
  SDL_ResumeAudioStreamDevice(stream);
  SDL_ShowWindow(win);

  bool running = 1;
  float *chunk = malloc(CHUNK_SAMPLES * sizeof(float));
  float *out = malloc(VISUAL_SAMPLES * sizeof(float));
  RingBuffer vis;
  rb_init(&vis, 8192);

  // Main loop
  while (running) {
    SDL_Event e;
    // On poll les events pour fermer le programme
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT)
        running = false;
      // handle input to change freq, etc.
    }

    Osc osc;
    osc.phase = 0.0;
    osc.freq = 1000;
    float phase_inc = sin(M_PI * 2 * osc.freq / SAMPLE_RATE);

    for (int i = 0; i < CHUNK_SAMPLES; i++) {
      float r = sin(osc.phase);
      // if (r > 0) {
      //   r = 1;
      // } else if (r < 0) {
      //   r = -1;
      // } else if (r == 0) {
      //   r = 0;
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
    SDL_PutAudioStreamData(stream, chunk, CHUNK_SAMPLES);
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
    for (int x = 0; x < samples_to_draw; x++) {
      float r = vis.buf[(start + x) & vis.mask];
      int y = (int)(h * 0.5f - r * (h * 0.2f));
      SDL_RenderPoint(ren, (float)x, (float)y);
    }
    SDL_RenderPresent(ren);
  }

  free(chunk);
  return 0;
}
