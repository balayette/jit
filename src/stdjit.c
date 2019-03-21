#include "stdjit.h"
#include "log.h"
#include <stdlib.h>
#include "assert.h"

void libjit_free(void *ptr)
{
	free(ptr);
}

void *libjit_malloc(size_t size)
{
	void *ret = malloc(size);
	ASSERT(ret != NULL, "malloc(%zu) failed\n", size);

	return ret;
}

void *libjit_calloc(size_t nmemb, size_t size)
{
	void *ret = calloc(nmemb, size);
	ASSERT(ret != NULL, "calloc(%zu, %zu) failed\n", nmemb, size);

	return ret;
}

void *libjit_realloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
	ASSERT(ret != NULL, "realloc(%p, %zu) failed\n", ptr, size);

	return ret;
}

void *libjit_reallocarray(void *ptr, size_t nmemb, size_t size)
{
	size_t mult = nmemb * size;
	ASSERT(nmemb != 0 && mult / nmemb != size,
	       "reallocarray(%p, %zu, %zu) failed, size overflow", ptr, nmemb,
	       size);

	void *ret = realloc(ptr, mult);
	ASSERT(ret != NULL, "reallocarray(%p, %zu, %zu) failed\n", ptr, nmemb,
	       size);

	return ret;
}
