SHELL=/bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c11 -D_GNU_SOURCE
LDFLAGS = 

SRC = src

VGFLAGS = --leak-check=full --track-origins=yes

OBJS = $(SRC)/main.o $(SRC)/input.o \
       $(SRC)/parse.o $(SRC)/group.o $(SRC)/array.o

.PHONY: all clean valgrind

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $^ -o $@

valgrind: similar_lines tests/podobnawe_wiersze.in
	valgrind $(VGFLAGS) ./similar_lines <tests/podobnawe_wiersze.in

main.o: main.c array.h input.h parse.h group.h
array.o: array.c array.h
input.o: input.c array.h input.h parse.h
parse.o: parse.c array.h
group.o: group.c array.h parse.h group.h

clean:
	-rm $(OBJS)
	-rm similar_lines
