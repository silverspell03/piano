#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <bits/time.h>
#include <setjmp.h>
#define SDL_MAIN_HANDLED

#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum WAVE_FORM{
  SINE_FORM,
  SQUARE_FORM,
  TRIANGLE_FORM,
  SAW_FORM 
};

struct WaveCtx {
  double volume;
  double phase;
  int frequency;
  int sample_rate;
};

static float now_secondsf() {
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);

  return (float)(tp.tv_sec + tp.tv_nsec / 1000000000.0);
}


static void GenerateSineWave(struct WaveCtx *ctx, void *buf, SDL_AudioStream *stream, int samples) 
{
  float *data; //On crée un pointeur float data qui pointera vers l'adresse du buffer 
  data = buf;

  const float phase_inc = M_PI * 2 * 440 / (float)ctx->sample_rate;
/bin/bash: ligne 1: q : commande introuvable
  for (int i = 0; i < samples; i++)
  {
    if (ctx->phase >= M_PI * 2) ctx->phase = - M_PI * 2;
    float r = ctx->volume * sin(ctx->phase);
    ctx->phase += phase_inc;
    data[i] = r;
  }

  SDL_PutAudioStreamData(stream, buf, samples);
}


static void SDLCALL GenerateNoise(void *userdata, SDL_AudioStream *stream,
                                  int samples, int total_amount) {
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;

  float *buf = malloc(samples * sizeof(float));
  if (!buf)
    return;

  for (int i = 0; i < samples; i++) {
    float r = (float)rand() / (float)RAND_MAX;
    buf[i] = (r * 2.0f - 1.0f) * 0.2f; /* 0.2 = volume */
  }

  SDL_PutAudioStreamData(stream, buf, samples);
}

static void GenerateNote(enum WAVE_FORM form, void *buf, SDL_AudioStream *stream, void *ctx, int samples)
{
  ctx = (struct WaveCtx *)ctx;
  SDL_AudioSpec spec;
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;
  
  switch(form)
  {
    case SINE_FORM:
      GenerateSineWave(ctx, buf, stream, samples);
  }
}

static void InitAudio() {
  if (!SDL_Init(SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "SDL_INIT failed :%s\n", SDL_GetError());
  };
}

static void InitVideo() {
  if (!SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_INIT failed :%s\n", SDL_GetError());
  };
}

static void VisualizeData(const void *buf, int len) {}

int main() {
  srand(time(NULL)); // seed with current time
  // InitVideo();
  // SDL_Renderer *renderer;
  // SDL_Window *window;

  // SDL_CreateWindowAndRenderer("Piano", 900, 600, 0, &window, &renderer);
  // SDL_FRect rect = {100.0, 100.0, 50.0, 50.0};
  // SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  // SDL_RenderRect(renderer, &rect);
  // SDL_RenderClear(renderer);

  InitAudio();

  SDL_AudioSpec spec = {SDL_AUDIO_F32, 1, 44100};

  /* Open a playback stream with a callback that will feed the stream */
  SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, /* device id (default playback) */
    &spec,                             /* our app format */
      NULL,              /* callback to supply data */
      NULL                                 /* userdata */
  );

  if (!stream) {
    fprintf(stderr, "SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  struct WaveCtx ctx;
  ctx.volume = 0.2f;
  ctx.phase = 0.0f;
  ctx.frequency = 440.0f;
  ctx.sample_rate = spec.freq;
  
  int samples = 1000;
  float *buf = malloc(samples * sizeof(float));

  GenerateNote(SINE_FORM, buf, stream, &ctx, samples);
  SDL_SetAudioStreamPutCallback(stream, SDL_AudioStreamCallback callback, void *userdata)
  SDL_PutAudioStreamData(stream, buf, samples * sizeof(float));

  /* Start playback of the stream's device */
  if (!SDL_ResumeAudioStreamDevice(stream)) printf("%s\n", SDL_GetError());

  /* let it play for ~5 seconds */
  SDL_Delay(2000);

  /* cleanup */
  SDL_DestroyAudioStream(stream); /* closes device too */
  SDL_Quit();
  free(buf);
  return 0;
}
