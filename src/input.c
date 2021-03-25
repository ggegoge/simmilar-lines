#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

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
 *  @is_eof i @is_comment to boolowskie wyznaczniki mówiące (opowiednio):
 *  czy wejście się już skończyło (eof) bądź czy ten wiersz jest komentarzem.
 *  Zwraca długość obecnie wczytanej linii.
 */
static ssize_t read_line(char** line_ptr, size_t* line_size, bool* is_eof,
                         bool* is_comment)
{
  ssize_t line_len;
  /* za pomocą c wysonduję czy to nie jest linia komentarna */
  char c = getc(stdin);

  if (c == EOF) {
    *is_eof = true;
    return EOF;
  }

  if ((*is_comment = c == '#')) {
    while (!feof(stdin) && (c = getc(stdin)) != '\n');

    return EOF;
  }

  ungetc(c, stdin);
  line_len = getline(line_ptr, line_size, stdin);
  *is_eof = line_len == EOF;

  if (!line_ptr || errno == ENOMEM)
    exit(1);

  return line_len;
}

void read_text(ParsedText* ptext)
{
  ParsedLine pline;
  ssize_t line_len;
  size_t line_num = 1;
  size_t line_size = 0;
  char* line = NULL;
  bool is_comment = false, is_eof = false;

  while (!feof(stdin) && !is_eof) {
    line_len = read_line(&line, &line_size, &is_eof, &is_comment);

    if (!is_comment && !is_eof) {
      pline = parse_line(line, line_num, (size_t) line_len);

      if (pline.well_formed) {
        if (ptext->len == 0)
          array_init(ptext, sizeof(ParsedLine), BIG_ARRAY_LENGTH);

        array_append(ptext, sizeof(ParsedLine), &pline);
      }
    }

    ++line_num;
  }

  free(line);
}
