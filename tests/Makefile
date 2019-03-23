CFLAGS = -Wall -Wextra -std=c99 -g -MMD
CPPFLAGS = -I../include/
LDFLAGS = -lcriterion

TESTS = \
	health_check.c \
	free_ast.c \
	evaluate.c \
	ctx.c \

TESTS_OBJ = $(TESTS_SRC:.c=.o)
TESTS_EXE = $(addsuffix .test,$(basename $(TESTS)))

VALGRIND = valgrind --error-exitcode=42 --leak-check=full -q

.PRECIOUS: %.o
%.test: %.o ../libjit.a
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

.PHONY: all
all: $(TESTS_EXE)
	@for I in $(TESTS_EXE); 		\
	do 					\
		echo -e "\e[93mRunning test $$I\e[39m"; 	\
		 ./$$I && echo -e "\e[92mTest successful" || (echo -e "\e[91mTest failed"); 	\
	done; \
	echo -e "\e[93mDone running tests.\e[39m"

.PHONY: clean
clean:
	$(RM) $(TESTS_OBJ)
	$(RM) $(TESTS_EXE)
	$(RM) $(TESTS:.c=.d)
