#include "ringbuffer.h"
#include <stdio.h>
#include <stdlib.h>

int rb_init(RingBuffer *rb, size_t size_pow2) {
  rb->size = size_pow2;
  rb->buf = malloc(rb->size * sizeof(sample_t));
  rb->head = 0;
  rb->mask = rb->size - 1;
  return 0;
}

int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n) {
  for (int i = 0; i < n; i++) {
    rb->buf[(i + rb->head) & rb->mask] = in[i];
  }
  rb->head += n;
  return 0;
}

int rb_read_block(const RingBuffer *rb, sample_t *out, size_t n) {
  for (int i = 0; i < n; i++) {
    out[i] = rb->buf[i - rb->head & rb->mask];
  }
  return 0;
}

int rb_free(RingBuffer *rb) {
  free(rb->buf);
  rb->buf = NULL;
  return 0;
}
