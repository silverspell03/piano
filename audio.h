typedef struct Osc Osc;

struct Osc {
  float phase;
  float freq;
};

void GenerateNoise(void *buf, int samples);
