#ifndef LOG_H
#define LOG_H

#include <err.h>
#include <stdio.h>

#ifdef JIT_DEBUG
#define LIBJIT_DBG(...)                                                        \
	do {                                                                   \
		fprintf(stderr, "%s:%d | ", __FILE__, __LINE__);               \
		fprintf(stderr, __VA_ARGS__);                                  \
	} while (0)

#define LIBJIT_DBGNV(...)                                                      \
	do {                                                                   \
		fprintf(stderr, __VA_ARGS__);                                  \
	} while (0);
#else
#define LIBJIT_DBG(...)
#define LIBJIT_DBGNV(...)
#endif

#define LIBJIT_DIE(...)                                                        \
	do {                                                                   \
		LIBJIT_DBG(__VA_ARGS__);                                       \
		err(1, NULL);                                                  \
	} while (0)

#define LIBJIT_DIENV(...)                                                      \
	do {                                                                   \
		LIBJIT_DBGNV(__VA_ARGS__);                                     \
		err(1, NULL);                                                  \
	} while (0)

#endif /* !LOG_H */
