#include "ringbuffer.h"
#include <stdio.h>
#include <stdlib.h>

// On initie le ringbuffer, la taille doit être une puissance de 2
int rb_init(RingBuffer *rb, size_t size_pow2) {
  rb->size = size_pow2;
  rb->buf = calloc(rb->size, sizeof(sample_t));
  if (!rb->buf) {
    printf("Issue malloc vrb buffer.");
    return -1;
  }
  rb->head = 0;
  rb->mask = rb->size - 1;
  return 0;
}

// Le & effectue une opération bitwise, ça marche comme ça les ringbuffers
int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n) {
  for (int i = 0; i < n; i++) {
    rb->buf[(i + rb->head) & rb->mask] = in[i];
  }
  rb->head += n;
  return 0;
}

int rb_read_block(const RingBuffer *rb, sample_t *out, size_t n) {
  for (int i = 0; i < n; i++) {
    // AVANT : le signe est inversé et la formule ignore n, donc on ne lit
    // pas la fenêtre des n derniers échantillons réellement écrits
    // (aliasing / mauvaise portion du buffer relue).
    // out[i] = rb->buf[i - rb->head & rb->mask];

    // APRES : on relit les n derniers échantillons écrits par
    // rb_write_block, qui écrit à (head + i) & mask.
    out[i] = rb->buf[(rb->head - n + i) & rb->mask];
  }
  return 0;
}

int rb_free(RingBuffer *rb) {
  free(rb->buf);
  rb->buf = NULL;
  return 0;
}
