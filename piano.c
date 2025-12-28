#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "vendored/tinyexp/tinyexpr.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <bits/time.h>
#include <setjmp.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const float fps = 60.0;

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
};

struct AppCtx {
  struct WaveCtx *wavectx;
  struct expression *expr;
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_AudioStream *stream;
  SDL_AudioSpec spec;
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

  const float phase_inc = M_PI * 2 * 440 / 44100.0f;
  
  for (int i = 0; i < samples; i++)
  {
    if (ctx->phase > M_PI * 2) ctx->phase = - M_PI * 2;
    float r = ctx->volume * sin(ctx->phase);
    ctx->phase += phase_inc;
    data[i] = r;
  }

  SDL_PutAudioStreamData(stream, buf, samples);
}

static void GenerateSquareWave(struct WaveCtx *ctx, void *buf, SDL_AudioStream *stream, int samples) 
{
  float *data; //On crée un pointeur float data qui pointera vers l'adresse du buffer 
  data = buf;

  const float phase_inc = M_PI * 2 * 440 / 44100.0f;
  
  for (int i = 0; i < samples; i++)
  {
    if (ctx->phase > M_PI * 2) ctx->phase = - M_PI * 2;
    float r = ctx->volume * sin(ctx->phase);
    if (r > 0) r = 1.0 * ctx->volume;
    else if (r < 0) r = -1.0;
    else if (r == 0) r = 0.0;
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
  if (samples <= 0)
    return;

  int bytes_per_sample = sizeof(float);

  if (samples % bytes_per_sample != 0)
    return;
  
  switch(form)
  {
    case SINE_FORM:
      GenerateSineWave(ctx, buf, stream, samples);
    case SQUARE_FORM:
      GenerateSquareWave(ctx, buf, stream, samples);
  }
}


static void DrawData(SDL_Renderer *renderer, SDL_Window *window, void *buf, int samples, SDL_AudioSpec *spec)
{
  float *data = buf;
  int size_data = sizeof(float);;
  
  float x, y;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  float scale_x = 1.0; 
  float scale_y = 100.0;
  SDL_FPoint points[samples];

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (int i = 0; i < samples; i++)
  {
    x = (float)i - w / 2.0;
    y = (data[i] + h / 2.0) * scale_y;
    SDL_FPoint val = {x, y};
    points[i] = val;
  }
  
  SDL_RenderPoints(renderer, points, samples);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  // if (argc !=2)
  // {
  //   printf("Usage: %s <expression>", argv[0]);
  //   return 0;
  // }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_AUDIO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }


  srand(time(NULL)); // seed with current time
  struct AppCtx *appctx = calloc(1, sizeof(*appctx));
  *appstate = appctx;
  if (!SDL_CreateWindowAndRenderer("Piano!", 900, 600, 0, &appctx->window,
                                   &appctx->renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderVSync(appctx->renderer, 1);
  appctx->spec.format = SDL_AUDIO_F32;
  appctx->spec.channels = 1;
  appctx->spec.freq = 44100;
  appctx->wavectx = calloc(1, sizeof(struct WaveCtx));
  appctx->wavectx->volume = 0.1f;
  appctx->wavectx->phase = 0.0f;
  appctx->wavectx->frequency = 440.0f;

  SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, /* device id (default playback) */
      &appctx->spec,                             /* our app format */
      NULL,                              /* callback to supply data */
      NULL                               /* userdata */
  );
	
  if (!stream) {
      fprintf(stderr, "SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
      SDL_Quit();
      return 1;
  }
  appctx->stream = stream;

  SDL_SetAppMetadata("Piano!", "1.0", "org.jospeh.piano");

  /* Start playback of the stream's device */
  SDL_ResumeAudioStreamDevice(stream);
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {

  struct AppCtx *appctx = (struct AppCtx *)appstate;
  SDL_Renderer *ren = appctx->renderer;
  SDL_Window *win = appctx->window;
  SDL_AudioStream *stream = appctx->stream;

  struct WaveCtx *wavectx = appctx->wavectx;
  int samples = fps * appctx->spec.freq;
  
  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(ren);


  float *buf = malloc(samples * sizeof(float));

  GenerateNote(SINE_FORM, buf, stream, wavectx, samples);
  
  SDL_PutAudioStreamDataNoCopy(stream, buf, samples, NULL, NULL);

  /* Start playback of the stream's device */
  if (!SDL_ResumeAudioStreamDevice(stream)) printf("%s\n", SDL_GetError());
  DrawData(ren, win, buf, samples, &appctx->spec);
  SDL_RenderPresent(ren);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
