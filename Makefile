all: similar_lines;

similar_lines: main.o input.o parse.o group.o
	gcc -Wall -Wextra -Wpedantic -pedantic -std=c99 -o similar_lines main.o input.o parse.o group.o

main.o: main.c
	gcc -Wall -Wextra -Wpedantic -pedantic -std=c99 -c main.c -o main.o

input.o: input.c input.h
	gcc -Wall -Wextra -Wpedantic -pedantic -std=c99 -c input.c -o input.o

parse.o: parse.c parse.h
	gcc -Wall -Wextra -Wpedantic -pedantic -std=c99 -c parse.c -o parse.o

group.o: group.c group.h
	gcc -Wall -Wextra -Wpedantic -pedantic -std=c99 -c group.c -o group.o

clean:
	-rm *.o
	-rm similar_lines
