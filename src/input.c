#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include "array.h"
#include "parse.h"
#include "input.h"

/**
 * Modulik do wczytywania. */

#define COMMENT_MARKER '#'

/**
 *  Wczytywanie z stdinu aż do '\n' lub eofa
 *  line_ptr bufor do zapisywania linijek,
 *  line_size obecny rozmiar bufora (getline wymaga tych dwu rzeczy),
 *  is_eof i is_comment mówią o końcu linii bądź jej komentarności.
 *  Zwraca długość obecnie wczytanej linii bądź eof w razie linii pomijalnej. */
static ssize_t read_line(char** line_ptr, size_t* line_size, bool* is_eof,
                         bool* is_comment)
{
  ssize_t line_len;
  /* za pomocą c wysonduję czy to nie jest linia komentarna pierwien niźli ją
   * wczytam getline'em */
  int c = getc(stdin);

  if (c == EOF) {
    *is_eof = true;
    return EOF;
  }

  if ((*is_comment = c == COMMENT_MARKER)) {
    /* w przypadku komentarza nie chcemy ładować linii do getline'a, więc
     * jedziemy aż do końca linii */
    while (!feof(stdin) && getc(stdin) != '\n');

    return EOF;
  }

  ungetc(c, stdin);
  line_len = getline(line_ptr, line_size, stdin);
  *is_eof = line_len == EOF;

  if (!line_ptr || errno == ENOMEM || errno == EOVERFLOW)
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
  bool is_comment = false;
  bool is_eof = false;

  while (!feof(stdin) && !is_eof) {
    line_len = read_line(&line, &line_size, &is_eof, &is_comment);

    if (!is_comment && !is_eof) {
      pline = parse_line(line, line_num, (size_t)line_len);

      /* dodajemy nową linijkę do tablicy przetworzonych wtw gdy nie wykryto
       * z nią żadnych błędów w module parse'owawczym */
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
