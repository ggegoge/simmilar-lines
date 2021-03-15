#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "input.h"
#include "parse.h"

/**
 * A tiny module for reading next lines from the stdin.
 */

/**
 *  read the current line up to the '\n' or EOF.
 *  if it is a comment line (ie starts with '#') then move to the end without
 *  reading it into the memory.
 *  Params:
 *  @line_ptr is buffer to save the line in,
 *  @len is the place to store buffer's size,
 *  @is_eof and @is_comm store the information whether the line has ended and
 *  if it is a commentary line perhaps.
 */
void readln(char** line_ptr, size_t* len, bool* is_eof, bool* is_comm)
{
  char c = getc(stdin);

  if ((*is_comm = c == '#')) {
    /* move to the eol (eof if no eol) */
    printf("istotnie, komentarz\n");

    while (!feof(stdin) && (c = getc(stdin)) != '\n');
  }

  ungetc(c, stdin);
  *is_eof = getline(line_ptr, len, stdin) == -1;

  if (!line_ptr)
    exit(1);
}
