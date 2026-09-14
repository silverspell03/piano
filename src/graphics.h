#pragma once

#include "app.h"
#include <SDL3/SDL.h>

/**
 * @brief Dessine un cercle via l'algorithme de Bresenham.
 *
 * @param ren Renderer SDL cible.
 * @param cx  Coordonnée X du centre.
 * @param cy  Coordonnée Y du centre.
 * @param r   Rayon du cercle en pixels.
 *
 * @return void. Dessine directement sur `ren`, ne présente pas le frame
 *         (pas d'appel à SDL_RenderPresent ici).
 */
void draw_circle(SDL_Renderer *ren, float cx, float cy, float r);

/**
 * @brief Lit les derniers échantillons audio du ring buffer et les
 *        dessine sous forme de courbe, puis présente le frame.
 *
 * @param renderer  Renderer utilisé.
 * @param samples   Nombre d'échantillons à lire et dessiner.
 * @param vrb       Ring buffer contenant les échantillons audio.
 *
 * @return void.
 *
 * @warning Alloue deux VLA (out_buf, points) de taille `samples` sur la
 *          pile : `samples` doit rester borné par l'appelant pour éviter
 *          un stack overflow.
 * @note Applique un facteur d'échelle fixe (* 400) qui suppose un signal
 *       normalisé entre -1 et 1 ; à ajuster si le gain du signal source
 *       change (voir app_update).
 */
void draw_data(App *app, int samples, RingBuffer *vrb);
