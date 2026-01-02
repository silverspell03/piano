#include "audio.h"
#include "SDL3/SDL_audio.h"
#include "app.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

AudioCtx *create_audio() {

  // Ouverture du stream audio lié au device par défaut
  app->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          spec, NULL, NULL);
  if (!app->stream) {
    return NULL;
  }
  SDL_ResumeAudioStreamDevice(app->stream);
  return 0;
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
