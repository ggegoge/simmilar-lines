SHELL=/bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -O2

SRC = main.c input.c parse.c group.c array.c compare.c
OBJS = $(SRC:%.c=src/%.o)

.PHONY: all clean

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $^ -o $@

src/main.o: src/main.c src/array.h src/input.h src/parse.h src/group.h
src/array.o: src/array.c src/array.h
src/input.o: src/input.c src/input.h src/parse.h src/array.h
src/parse.o: src/parse.c src/parse.h src/array.h
src/group.o: src/group.c src/group.h src/compare.h src/parse.h src/array.h
src/compare.o: src/compare.c src/compare.h src/parse.h

clean:
	-rm -f $(OBJS)
	-rm -f similar_lines
