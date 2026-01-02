#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
typedef struct UIContext UIContext;
typedef struct Widget Widget;

UIContext *create_ui();
void free_ui(UIContext *ui);
void draw_ui(UIContext *ui);
