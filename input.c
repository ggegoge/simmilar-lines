#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "array.h"
#include "parse.h"
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
static void readln(char** line_ptr, size_t* len, bool* is_eof, bool* is_comm)
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

void read_text(PText* ptext)
{
  size_t line_num = 1;
  size_t len = 0;
  char* line = NULL;
  bool is_comm = false, is_eof = false;
  PLine pline;

  while (!feof(stdin) && !is_eof) {
    readln(&line, &len, &is_eof, &is_comm);

    if (!is_comm && !is_eof) {
      pline = parseln(line, line_num);

      if (pline.well_formed) {
        if (ptext->len == 0)
          init(ptext, sizeof(PLine), BIG_ARRAY);

        append(ptext, sizeof(PLine), &pline);
      }
    }

    ++line_num;
  }

  free(line);
}
