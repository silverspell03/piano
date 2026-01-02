#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
typedef struct UIContext UIContext;
typedef struct Widget Widget;

UIContext *create_ui(SDL_Renderer *ren);

void ui_on_resize(UIContext *ui, int w,
                  int h); // Mise a jour de la taille de la fenetre de notre ui.
void show_ui(UIContext *ui);
void ui_destroy(UIContext *ui);
void draw_ui(UIContext *ui);
