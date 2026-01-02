#include "SDL3/SDL_audio.h"
#include <SDL3/SDL.h>

typedef struct Osc Osc;
typedef struct AudioCtx AudioCtx;

struct Osc {
  float phase;
  float freq;
};

typedef struct {
  float freq; // e.g. 440
  int sample_rate;
  SDL_AudioFormat format; // e.g. SDL_AUDIO_F32SYS
  int channels;           // 1 or 2
  int samples;            // preferred device buffer size (chunk)
} AudioConfig;

AudioCtx *create_audio(AudioConfig *cfg);
void GenerateNoise(void *buf, int samples);
