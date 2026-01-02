#pragma once
#include <stddef.h>
typedef float sample_t;

typedef struct RingBuffer RingBuffer;

struct RingBuffer {
  sample_t *buf;
  size_t size; // power-of-two recommended
  size_t head; // write index (next write)
  size_t mask; // size - 1
};

int rb_init(RingBuffer *rb, size_t size_pow2);
int rb_write_block(RingBuffer *rb, const sample_t *in, size_t n);
int rb_read_block(const RingBuffer *rb, sample_t *out, size_t n);
int rb_free(RingBuffer *rb);
