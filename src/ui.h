#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
typedef struct UIContext UIContext;
typedef struct Widget Widget;

/**
 * @brief Crée un contexte UI associé à un renderer.
 *
 * @param ren Renderer SDL utilisé pour le dessin de l'UI.
 *
 * @return Pointeur vers un UIContext alloué, ou NULL en cas d'échec.
 *
 * @warning Déclarée mais non définie actuellement : tout appel provoquera
 *          une erreur de link si utilisée.
 * @note Propriété mémoire : l'appelant devient propriétaire du UIContext
 *       retourné et doit appeler ui_destroy() pour le libérer.
 */
UIContext *create_ui(SDL_Renderer *ren, int w, int h);

/**
 * @brief Met à jour les dimensions connues par l'UI (ex: après un resize
 *        de fenêtre).
 *
 * @param ui UI à mettre à jour, modifiée en place.
 * @param w  Nouvelle largeur.
 * @param h  Nouvelle hauteur.
 *
 * @return void.
 */
void ui_on_resize(UIContext *ui, int w, int h);

/**
 * @brief Affiche la fenêtre associée à l'UI.
 *
 * @param ui UI dont la fenêtre doit être affichée.
 *
 * @return void.
 */
void show_ui(UIContext *ui);

/**
 * @brief Libère le contexte UI.
 *
 * @param ui UI à libérer.
 *
 * @return void.
 *
 * @warning free(ui) est appelé même si ui est NULL sans vérification
 *          préalable — free(NULL) est légal en C mais vérifier explicitement
 *          documente l'intention.
 */
void ui_destroy(UIContext *ui);

/**
 * @brief Dessine les éléments graphiques de l'UI (rectangle + cercle
 *        indicateur) sur le renderer associé.
 *
 * @param ui UI à dessiner, lecture des dimensions courantes (width,
 *           heigth) pour positionner les éléments.
 *
 * @return void. Ne présente pas le frame (pas de SDL_RenderPresent ici).
 */
void draw_ui(UIContext *ui);
