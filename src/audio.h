#include "SDL3/SDL_audio.h"
#include <SDL3/SDL.h>

#define SAMPLE_RATE 48000
#define CHANNELS 1

typedef struct Osc Osc;
typedef struct AudioCtx AudioCtx;

struct Osc {
  float phase;
  float freq;
};

typedef struct {
  float freq; // e.g. 440
  int sample_rate;
  SDL_AudioFormat format; // e.g. SDL_AUDIO_F32SYS
  int channels;           // 1 or 2
  int samples;            // preferred device buffer size (chunk)
} AudioConfig;

/**
 * @brief Crée un contexte audio et ouvre un stream vers le device de
 *        lecture par défaut.
 *
 * @param cfg Configuration audio (fréquence, format, canaux). Doit rester
 *            valide pendant l'appel ; non conservé au-delà.
 *
 * @return Pointeur vers un AudioCtx alloué, ou NULL si l'ouverture du
 *         stream échoue.
 *
 * @note Propriété mémoire : l'appelant devient propriétaire du AudioCtx
 *       retourné. Aucune fonction destroy_audio() n'existe actuellement
 *       pour le libérer proprement (à ajouter si cette fonction est
 *       utilisée en pratique).
 */
AudioCtx *create_audio(AudioConfig *cfg);

/**
 * @brief Remplit un buffer avec du bruit blanc aléatoire.
 *
 * @param buf     Buffer à remplir, doit être un tableau de float d'au
 *                moins `samples` éléments.
 * @param samples Nombre d'échantillons (pas d'octets) à générer.
 *
 * @return void.
 *
 * @warning Le nom du paramètre et le test interne (samples %
 *          bytes_per_sample) prêtent à confusion : `samples` est traité
 *          comme un nombre d'échantillons float, pas un nombre d'octets,
 *          malgré ce que suggère le test.
 */
void GenerateNoise(void *buf, int samples);
