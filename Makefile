SHELL=/bin/sh

.PHONY: all clean valgrind

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c11 -D_GNU_SOURCE
LDFLAGS = 

VGFLAGS = --leak-check=full --track-origins=yes

OBJS = main.o input.o parse.o group.o array.o

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $^ -o $@

valgrind: similar_lines tests/my_ex.in
	valgrind $(VGFLAGS) ./similar_lines <tests/my_ex.in

main.o: main.c array.h input.h parse.h group.h
array.o: array.c array.h
input.o: input.c array.h input.h parse.h
parse.o: parse.c array.h group.h
group.o: group.c array.h parse.h group.h

clean:
	-rm $(OBJS)
	-rm similar_lines
