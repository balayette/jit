#ifndef STDJIT_H
#define STDJIT_H

#include <stddef.h>

void libjit_free(void *ptr);
void *libjit_malloc(size_t size);
void *libjit_calloc(size_t nmemb, size_t size);
void *libjit_realloc(void *ptr, size_t size);
void *libjit_reallocarray(void *ptr, size_t nmemb, size_t size);

#endif /* STDJIT_H */
