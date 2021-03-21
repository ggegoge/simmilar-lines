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
 *  @line_size obecny rozmiar bufora,
 *  @is_eof i @is_comm to boolowskie wyznaczniki mówiące (opowiednio):
 *  czy wejście się już skończyło (eof) bądź czy ten wiersz jest komentarzem.
 */
static ssize_t readln(char** line_ptr, size_t* line_size, bool* is_eof, bool* is_comm)
{
  char c = getc(stdin);
  ssize_t line_len;

  if ((*is_comm = c == '#')) {
    while (!feof(stdin) && (c = getc(stdin)) != '\n');
    return -1;
  }

  ungetc(c, stdin);
  line_len = getline(line_ptr, line_size, stdin);
  *is_eof = line_len == -1;

  if (!line_ptr)
    exit(1);

  return line_len;  
}

void read_text(PText* ptext)
{
  size_t line_num = 1;
  size_t line_size = 0;
  ssize_t line_len;
  char* line = NULL;
  bool is_comm = false, is_eof = false;
  PLine pline;

  while (!feof(stdin) && !is_eof) {
    line_len = readln(&line, &line_size, &is_eof, &is_comm);

    if (!is_comm && !is_eof) {
      pline = parseln(line, line_num, (size_t)line_len);

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
