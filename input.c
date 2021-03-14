#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "input.h"
#include "parse.h"

/**
 * A module for reading next lines from the stdin and then neatly packing them
 * into dynamic arrays DText and DLine
 */

size_t new_len(size_t curr_len)
{
  return ((size_t)curr_len + (curr_len >> 2) + 6) & ~(size_t)3;
  /* return (size_t) ceil(curr_len * ARR_EXPAND); */
}

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
    while (!feof(stdin) && getc(stdin) != '\n');

  }

  ungetc(c, stdin);
  *is_eof = getline(line_ptr, len, stdin) == -1;

  if (!line_ptr)
    exit(1);
}
