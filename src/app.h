#pragma once

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "audio.h"
#include "ringbuffer.h"
#include "ui.h"
#include <SDL3/SDL.h>

typedef struct AudioCtx AudioCtx;
typedef struct RingBuffer RingBuffer;

typedef struct App {
  /* Pointers (64-bit / 8 bytes on 64-bit platforms) */
  UIContext *uictx;
  SDL_Window *win;
  SDL_Renderer *ren;
  AudioCtx *actx;
  SDL_AudioStream *stream;
  void *a_buf;
  RingBuffer *vrb;

  /* Embedded structs / Larger types */
  Osc osc;

  /* 32-bit integers (4 bytes) */
  int width;
  int height;

  /* Booleans / Flags (1 byte) */
  bool running;
} App;

/**
 * @brief Crée le contexte applicatif complet : fenêtre, renderer, stream
 *        audio, oscillateur et ring buffer de visualisation.
 *
 * @param freq Fréquence initiale de l'oscillateur en Hz.
 *
 * @return Pointeur vers un App alloué, ou NULL si une étape d'initialisation
 *         échoue (fenêtre ou ring buffer).
 *
 * @note Propriété mémoire : l'appelant devient propriétaire de l'App
 *       retourné et doit appeler destroy_app() pour tout libérer
 *       (ring buffer, stream, fenêtre, renderer).
 * @warning En cas d'échec après la création de la fenêtre/stream (ex:
 *          rb_init échoue), ces ressources ne sont pas explicitement
 *          libérées avant le `return NULL` — fuite potentielle à vérifier.
 */
App *create_app(float freq);

/**
 * @brief Traite les événements SDL en attente (quit, resize, clic souris)
 *        et met à jour l'état de l'application en conséquence.
 *
 * @param app Contexte applicatif, modifié en place (running, width,
 *            height, éventuellement osc.freq selon les événements).
 *
 * @return bool, retourne la valeur de app->running.
 */
bool app_handle_event(App *app);

/**
 * @brief Avance la simulation audio d'un pas de temps dt, génère les
 *        échantillons correspondants, les envoie au device audio, les
 *        stocke dans le ring buffer, et déclenche le dessin.
 *
 * @param app Contexte applicatif, modifié en place (osc.phase avancée,
 *            ring buffer alimenté).
 * @param dt  Delta de temps écoulé depuis le dernier appel, en secondes.
 *
 * @return void.
 *
 * @warning dt non clampé peut produire un nombre d'échantillons (VLA)
 *          disproportionné en cas de freeze de la boucle principale.
 */
void app_update(App *app, float dt);

/**
 * @brief Dessine l'état courant de l'application.
 *
 * @param app Contexte applicatif à dessiner.
 *
 * @return void.
 *
 * @warning Déclarée dans app.h mais non définie dans app.c actuellement.
 *          Le dessin se fait pour l'instant directement via draw_data(),
 *          appelée depuis app_update().
 */
void app_draw(App *app);

/**
 * @brief Libère toutes les ressources du contexte applicatif.
 *
 * @param app Contexte à détruire. Peut être NULL (retourne -1 dans ce cas).
 *
 * @return 0 en cas de succès, -1 si app est NULL.
 *
 * @warning Appelle ui_destroy(app->uictx) même si uictx n'a jamais été
 *          assigné (reste NULL depuis calloc) — sans effet néfaste
 *          (free(NULL) est légal) mais révèle que create_ui() n'est
 *          jamais appelée dans create_app().
 */
int destroy_app(App *app);
