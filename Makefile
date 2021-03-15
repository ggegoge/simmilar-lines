SHELL=/bin/sh

.PHONY: all clean valgrind

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c99 -D_GNU_SOURCE

all: similar_lines;

similar_lines: main.o input.o parse.o group.o
	$(CC) $(CFLAGS) -o similar_lines main.o input.o parse.o group.o

.c.o:
	$(CC) $(CFLAGS) -c $<

valgrind: similar_lines my_ex.in
	valgrind --leak-check=full --track-origins=yes ./similar_lines <my_ex.in

# main.o: main.c
# 	$(CC) $(CFLAGS) -c main.c

# input.o: input.c input.h
# 	$(CC) $(CFLAGS) -c input.c

# parse.o: parse.c parse.h
# 	$(CC) $(CFLAGS) -c parse.c

# group.o: group.c group.h
# 	$(CC) $(CFLAGS) -c group.c

clean:
	-rm *.o
	-rm similar_lines
