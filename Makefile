SHELL=/bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pedantic -std=c11 -D_GNU_SOURCE
LDFLAGS = 

SRC = src

SOURCES = main.c input.c parse.c group.c array.c compare.c

OBJS = $(SOURCES:%.c=$(SRC)/%.o)

VGFLAGS = --leak-check=full --track-origins=yes

.PHONY: all clean valgrind

all: similar_lines;

similar_lines: $(OBJS)
	$(CC) $^ -o $@

valgrind: similar_lines tests/podobnawe_wiersze.in
	valgrind $(VGFLAGS) ./similar_lines <tests/podobnawe_wiersze.in

$(SRC)/main.o: $(SRC)/main.c $(SRC)/array.h $(SRC)/input.h $(SRC)/parse.h \
               $(SRC)/group.h
$(SRC)/array.o: $(SRC)/array.c $(SRC)/array.h
$(SRC)/input.o: $(SRC)/input.c $(SRC)/array.h $(SRC)/input.h $(SRC)/parse.h
$(SRC)/parse.o: $(SRC)/parse.c $(SRC)/array.h
$(SRC)/group.o: $(SRC)/group.c $(SRC)/array.h $(SRC)/parse.h \
                $(SRC)/compare.h $(SRC)/group.h
$(SRC)/compare.o: $(SRC)/compare.c $(SRC)/parse.h $(SRC)/compare.h

clean:
	-rm $(OBJS)
	-rm similar_lines
