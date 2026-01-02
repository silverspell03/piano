#include <stdlib.h>

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
