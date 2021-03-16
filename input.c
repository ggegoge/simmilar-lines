#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "input.h"

/**
 * Modulik do wczytywania.
 */

/**
 *  Wczytywanie z stdinu danych aż do końca linii ('\n' lub eof).
 *  @line_ptr bufor do zapisywania linijek,
 *  @len obecny rozmiar bufora,
 *  @is_eof i @is_comm to boolowskie wyznaczniki mówiące (opowiednio):
 *  czy wejście się już skończyło (eof) bądź czy ten wiersz jest komentarzem.
 */
void readln(char** line_ptr, size_t* len, bool* is_eof, bool* is_comm)
{
  char c = getc(stdin);

  if ((*is_comm = c == '#')) {
    while (!feof(stdin) && (c = getc(stdin)) != '\n');
  }

  ungetc(c, stdin);
  *is_eof = getline(line_ptr, len, stdin) == -1;

  if (!line_ptr)
    exit(1);
}
