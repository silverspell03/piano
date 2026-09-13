#pragma once
#include <stddef.h>
typedef float sample_t;

typedef struct RingBuffer {
  sample_t *buf;
  size_t size; // power-of-two recommended
  size_t head; // write index (next write)
  size_t mask; // size - 1
} RingBuffer;

/**
 * @brief Initialise un ring buffer et alloue son buffer interne.
 *
 * @param rb        Ring buffer à initialiser. Doit déjà être alloué par
 *                  l'appelant (sur la pile ou via malloc) ; cette fonction
 *                  n'alloue que rb->buf, pas la struct rb elle-même.
 * @param size_pow2 Taille du buffer en nombre d'échantillons. Doit être
 *                  une puissance de 2 (utilisé comme masque bitwise via
 *                  mask = size - 1).
 *
 * @return 0 en cas de succès, -1 si l'allocation de rb->buf échoue.
 *
 * @note Propriété mémoire : rb->buf est alloué ici et doit être libéré
 *       via rb_free(), pas via un simple free(rb).
 */
int rb_init(RingBuffer *rb, size_t size_pow2);

/**
 * @brief Écrit n échantillons dans le ring buffer à partir de head.
 *
 * @param rb Ring buffer cible, modifié en place (head avancé de n après
 *           l'écriture).
 * @param in Buffer source contenant au moins n échantillons.
 * @param n  Nombre d'échantillons à écrire.
 *
 * @return 0 (toujours — pas de gestion d'erreur actuellement).
 *
 * @warning Aucune vérification que n ne dépasse pas rb->size : écrire
 *          plus d'échantillons que la capacité du buffer entre deux
 *          lectures écrase des données non encore lues.
 */
int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n);

/**
 * @brief Relit les n derniers échantillons écrits dans le ring buffer.
 *
 * @param rb  Ring buffer source (lecture seule, non modifié).
 * @param out Buffer de destination, doit pouvoir contenir n échantillons.
 * @param n   Nombre d'échantillons à lire.
 *
 * @return 0 (toujours).
 *
 * @warning Suppose que n <= rb->size, sinon la fenêtre lue chevauche des
 *          données déjà écrasées par des écritures plus récentes.
 */

int rb_read_block(const RingBuffer *rb, sample_t *out, size_t n);

/**
 * @brief Libère le buffer interne d'un ring buffer.
 *
 * @param rb Ring buffer à libérer. rb->buf est mis à NULL après libération.
 *
 * @return 0 (toujours).
 *
 * @note Ne libère pas rb lui-même (la struct), seulement rb->buf.
 *       Voir la note de propriété mémoire dans rb_init().
 */
int rb_free(RingBuffer *rb);
