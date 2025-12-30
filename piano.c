#include <stddef.h>
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "vendored/tinyexp/tinyexpr.h"
#include <bits/time.h>
#include <setjmp.h>

#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLE_RATE 48000
#define CHANNELS 1
#define FPS 60
#define SAMPLES_PER_FRAME (SAMPLE_RATE / FPS)
#define VISUAL_SAMPLES (SAMPLE_RATE / 10) // show last 100 ms ~4800 samples

typedef float sample_t;

enum WAVE_FORM {
  SINE_FORM,
  SQUARE_FORM,
};

struct WaveCtx {
  double volume;
  double phase;
  int frequency;
};

struct AppCtx {
  struct WaveCtx *wavectx;
  struct expression *expr;
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_AudioStream *stream;
  SDL_AudioSpec spec;
};

typedef struct {
  sample_t *buf;
  size_t size; // power-of-two recommended
  size_t head; // write index (next write)
  size_t mask; // size - 1
} RingBuffer;

static int rb_init(RingBuffer *rb, size_t size_pow2) {
  rb->size = size_pow2;
  rb->buf = malloc(rb->size * sizeof(&rb));
  rb->head = 0;
  rb->mask = rb->size - 1;
  return 0;
}

static int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n) {
  for (int i = 0; i < n; i++) {
    rb->buf[i + rb->head & rb->mask] = in[i];
    rb->head++;
  }
  return 0;
}

static int rb_read_block(RingBuffer *rb, const sample_t *out, size_t n) {
  return 0;
}

static int rb_free(RingBuffer *rb) {
  free(rb->buf);
  rb->buf = NULL;
  return 0;
}

static float now_secondsf() {
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);

  return (float)(tp.tv_sec + tp.tv_nsec / 1000000000.0);
}

static void GenerateSineWave(struct WaveCtx *ctx, void *buf,
                             SDL_AudioStream *stream, int samples) {
  float *data; // On crée un pointeur float data qui pointera vers l'adresse du
               // buffer
  data = buf;

  const float phase_inc = M_PI * 2 * 440 / 44100.0f;

  for (int i = 0; i < samples; i++) {
    if (ctx->phase > M_PI * 2)
      ctx->phase = -M_PI * 2;
    float r = ctx->volume * sin(ctx->phase);
    ctx->phase += phase_inc;
    data[i] = r;
  }

  SDL_PutAudioStreamData(stream, buf, samples);
}

static void GenerateSquareWave(struct WaveCtx *ctx, void *buf,
                               SDL_AudioStream *stream, int samples) {
  float *data; // On crée un pointeur float data qui pointera vers l'adresse du
               // buffer
  data = buf;

  const float phase_inc = M_PI * 2 * 440 / 44100.0f;

  for (int i = 0; i < samples; i++) {
    if (ctx->phase > M_PI * 2)
      ctx->phase = -M_PI * 2;
    float r = ctx->volume * sin(ctx->phase);
    if (r > 0)
      r = 1.0 * ctx->volume;
    else if (r < 0)
      r = -1.0;
    else if (r == 0)
      r = 0.0;
    ctx->phase += phase_inc;
    data[i] = r;
  }

  SDL_PutAudioStreamData(stream, buf, samples);
}

static void GenerateNoise(void *buf, int samples) {
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;

  if (!buf)
    return;

  for (int i = 0; i < samples; i++) {
    float r = (float)rand() / (float)RAND_MAX;
    r = (r * 2.0f - 1.0f) * 0.2f; /* 0.2 = volume */
    rb_write_block(buf, &r, 1);
  }
}

static void GenerateNote(enum WAVE_FORM form, void *buf,
                         SDL_AudioStream *stream, void *ctx, int samples) {
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;

  switch (form) {
  case SINE_FORM:
    GenerateSineWave(ctx, buf, stream, samples);
  case SQUARE_FORM:
    GenerateSquareWave(ctx, buf, stream, samples);
  }
}

static void DrawData(SDL_Renderer *renderer, SDL_Window *window, void *buf,
                     int samples, SDL_AudioSpec *spec) 
{
  float *data = buf;
  int size_data = sizeof(float);

  float x, y;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  float scale_x = 1.0;
  float scale_y = 100.0;
  SDL_FPoint *points = malloc((size_t)samples * sizeof(*points));

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (int i = 0; i < samples; i++) {
    x = (float)i - w / 2.0;
    y = (data[i] + h / 2.0) * scale_y;
    SDL_FPoint val = {x, y};
    points[i] = val;
  }

  SDL_RenderPoints(renderer, points, samples);
  free(points);
}

/* This function runs once at startup. */
int main(int argc, char **argv) {

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }

  srand(time(NULL)); // seed with current time
  
  float *b_audio = malloc(SAMPLES_PER_FRAME * sizeof(float));

  GenerateNoise(b_audio, SAMPLES_PER_FRAME);

  RingBuffer rb;
  rb_init(&rb, 32);
  GenerateNoise(&rb, 8000000);
  for (int i = 0; i < rb.size; i++) {
    printf("i = %d: %f\n", i, rb.buf[i]);
  }
  rb_free(&rb);
  return 0;
}
