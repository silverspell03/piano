#include "app.h"

void handle_input(AppState *state)
{
  bool *running = &state->running;
  UIContext *ui = state->uictx;
  SDL_Event e;

  // On poll les events pour fermer le programme
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      float x, y;
      case SDL_EVENT_QUIT:
        *running = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_GetWindowSize(ui->win, &ui->width, &ui->heigth);
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (SDL_GetRelativeMouseState(&x, &y) != SDL_BUTTON_LEFT) {
          break;
        }
        else if () {
          
        }
    }
  // handle input to change freq, etc.
  }
}
