#include "audio.h"
#include "SDL3/SDL_audio.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

struct AudioCtx {
  SDL_AudioStream *stream;
  SDL_AudioSpec *spec;
  
};

AudioCtx *create_audio(AudioConfig *cfg) {

  AudioCtx *ctx = malloc(sizeof(*ctx));
  SDL_AudioSpec spec = {cfg->format, cfg->channels, cfg->freq};
  // Ouverture du stream audio lié au device par défaut
  ctx->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          &spec, NULL, NULL);
  if (!ctx->stream) {
    return NULL;
  }
  SDL_ResumeAudioStreamDevice(ctx->stream);
  return ctx;
}

void GenerateNoise(void *buf, int samples) {
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
