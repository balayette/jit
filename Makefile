CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -MMD
CPPFLAGS = -Iinclude/

.PHONY: all
all: opti

.PHONY: opti
opti: CFLAGS += -O3
opti: libjit.a

.PHONY: debug
debug: CFLAGS += -Og
debug: CPPFLAGS += -DJIT_DEBUG
debug: libjit.a

SRC = \
	src/ast.c \
	src/stdjit.c \

OBJ = $(SRC:.c=.o)

TEST_SRC = \
	tests/simple.c \

DEP = $(OBJ:.o=.d)

.PRECIOUS: $(OBJ)

libjit.a: $(OBJ)
	ar -rc libjit.a $(OBJ)

.PHONY: test
test: libjit.a
	$(MAKE) -C tests/

.PHONY: clean
clean:
	$(RM) libjit.a
	$(RM) $(OBJ)
	$(RM) $(DEP)
	$(MAKE) -C tests clean

-include $(DEP)
