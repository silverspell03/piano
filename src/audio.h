#include <SDL3/SDL.h>
typedef struct Osc Osc;
typedef struct AudioCtx AudioCtx;

struct Osc {
  float phase;
  float freq;
};

int create_audio();
void GenerateNoise(void *buf, int samples);
