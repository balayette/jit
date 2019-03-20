#include "stdjit.h"
#include "log.h"
#include <stdlib.h>

void libjit_free(void *ptr)
{
	free(ptr);
}

void *libjit_malloc(size_t size)
{
#ifndef JIT_DEBUG
	return malloc(size);
#else
	void *ret = malloc(size);
	if (!ret)
		LIBJIT_DIE("malloc(%zu) failed\n", size);

	return ret;
#endif
}

void *libjit_calloc(size_t nmemb, size_t size)
{
#ifndef JIT_DEBUG
	return calloc(nmemb, size);
#else
	void *ret = calloc(nmemb, size);
	if (!ret)
		LIBJIT_DIE("calloc(%zu, %zu) failed\n", nmemb, size);

	return ret;
#endif
}

void *libjit_realloc(void *ptr, size_t size)
{
#ifndef JIT_DEBUG
	return realloc(ptr, size);
#else
	void *ret = realloc(ptr, size);
	if (!ret)
		LIBJIT_DIE("realloc(%p, %zu) failed\n", ptr, size);

	return ret;
#endif
}

void *libjit_reallocarray(void *ptr, size_t nmemb, size_t size)
{
#ifndef JIT_DEBUG
	return realloc(ptr, nmemb * size);
#else
	size_t mult = nmemb * size;
	if (nmemb != 0 && mult / nmemb != size)
		LIBJIT_DIE("reallocarray(%p, %zu, %zu) failed, size overflow",
			   ptr, nmemb, size);

	void *ret = realloc(ptr, mult);
	if (!ret)
		LIBJIT_DIE("reallocarray(%p, %zu, %zu) failed\n", ptr, nmemb,
			   size);

	return ret;
#endif
}
