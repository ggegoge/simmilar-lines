SHELL=/bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c11 -D_GNU_SOURCE
LDFLAGS = 

SRC = main.c input.c parse.c group.c array.c compare.c
OBJS = $(SRC:%.c=src/%.o)

VGFLAGS = --leak-check=full --track-origins=yes

.PHONY: all clean valgrind

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $^ -o $@

valgrind: similar_lines tests/podobnawe_wiersze.in
	valgrind $(VGFLAGS) ./similar_lines <tests/podobnawe_wiersze.in

src/main.o: src/main.c src/array.h src/input.h src/parse.h src/group.h
src/array.o: src/array.c src/array.h
src/input.o: src/input.c src/input.h src/parse.h src/array.h
src/parse.o: src/parse.c src/parse.h src/array.h
src/group.o: src/group.c src/group.h src/compare.h src/parse.h src/array.h
src/compare.o: src/compare.c src/compare.h src/parse.h

clean:
	-rm $(OBJS)
	-rm similar_lines
