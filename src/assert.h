#ifndef ASSERT_H
#define ASSERT_H

#include "log.h"

#ifdef JIT_ASSERT
#define ASSERT(expr, ...)                                                      \
	do {                                                                   \
		if (!(expr)) {                                                 \
			LIBJIT_DBG("Assertion '%s' failed: ", #expr);          \
			LIBJIT_DIENV(__VA_ARGS__);                             \
		}                                                              \
	} while (0);
#else
#define ASSERT(expr, ...)
#endif

#endif /* !ASSERT_H */
