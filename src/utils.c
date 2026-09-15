#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "Fatal: Out of memory allocated (%zu bytes)\n", size);
    exit(EXIT_FAILURE);
  }
  return ptr;
}
