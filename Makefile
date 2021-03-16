SHELL=/bin/sh

.PHONY: all clean valgrind

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c99 -D_GNU_SOURCE

VGFLAGS = --leak-check=full --track-origins=yes

OBJS = main.o input.o parse.o group.o array.o

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $(CFLAGS) -o similar_lines $(OBJS)

valgrind: similar_lines tests/my_ex.in
	valgrind $(VGFLAGS) ./similar_lines <tests/my_ex.in

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

array.o: array.c array.h
	$(CC) $(CFLAGS) -c array.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

parse.o: parse.c parse.h
	$(CC) $(CFLAGS) -c parse.c

group.o: group.c group.h
	$(CC) $(CFLAGS) -c group.c

clean:
	-rm $(OBJS)
	-rm similar_lines
