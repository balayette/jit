CFLAGS = -Wall -Wextra -pedantic -std=c99 -g3 -MMD
CPPFLAGS = -Iinclude/

.PHONY: all
all: opti

.PHONY: opti
opti: CFLAGS += -O3
opti: libjit.a

.PHONY: opti-assert
opti-assert: CPPFLAGS += -DJIT_ASSERT
opti-assert: opti

.PHONY: debug
debug: CFLAGS += -Og
debug: CPPFLAGS += -DJIT_DEBUG -DJIT_ASSERT
debug: libjit.a

demo: libjit.a

SRC = \
	src/ast.c \
	src/stdjit.c \
	src/jit.c \
	src/ass.c \

OBJ = $(SRC:.c=.o)

DEP = $(OBJ:.o=.d)

.PRECIOUS: $(OBJ)

libjit.a: $(OBJ)
	ar -rc libjit.a $(OBJ)

.PHONY: test
test: opti-assert
	$(MAKE) -C tests/

.PHONY: clean
clean:
	$(RM) libjit.a
	$(RM) $(OBJ)
	$(RM) $(DEP)
	$(RM) demo
	$(MAKE) -C tests clean

-include $(DEP)
