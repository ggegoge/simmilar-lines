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

/* int append_to_line(DLine* line, char new_char)
 * {
 *   line->used++;
 * 
 *   if (line->used >= line->len) {
 *     line->len = new_len(line->len);
 *     line->val = (char*)realloc(line->val, line->len * sizeof(char));
 * 
 *     if (!line->val) {
 *       fprintf(stderr, "REALLOC FAILURE\n");
 *       return -1;
 *     }
 *   }
 * 
 *   line->val[line->used - 1] = new_char;
 *   return 0;
 * }
 * 
 * DLine init_line(size_t init_len, size_t line_num)
 * {
 *   DLine line;
 *   line.line_num = line_num;
 *   line.used = 0;
 *   line.len = init_len;
 *   line.val = (char*)malloc(init_len * sizeof(char));
 *   line.commentary = 0;
 * 
 *   if (!line.val)
 *     line.initialised = -1;
 * 
 *   return line;
 * }
 * 
 * bool read_line(size_t line_num, DLine* line)
 * {
 *   char c;
 *   bool alloc_status;
 *   line->line_num = line_num;
 * 
 *   if (scanf("%c", &c) != 1)
 *     return false;
 * 
 *   if (c == '#') {
 *     printf("to jest kom\n");
 *     line->commentary = 1;
 * 
 *     while (scanf("%c", &c) == 1 && c != '\n');
 * 
 *     return true;
 *   } else
 *     printf("fisrt char is: %c\n", c);
 * 
 *   do {
 *     alloc_status = append_to_line(line, c);
 *   } while (alloc_status == 0 && c != '\n' && scanf("%c", &c) == 1 &&
 *            !feof(stdin));
 * 
 *   return alloc_status;
 * } */

/**
 *  read the current line up to the '\n' or EOF.
 *  if it is a comment line (ie starts with '#') then move to the end without
 *  reading it into the memory.
 *  Params:
 *  @line_ptr is buffer to save the line in,
 *  @len is the place to store buffer's size,
 *  @is_eof and @is_comm store the information whether the line has ended and
 *  if it is a commentary line perhaps.
 *  The return value tells whether the func succeded memory wise. */
bool read_line_getv(char** line_ptr, size_t* len, bool* is_eof, bool* is_comm)
{
  char c = getc(stdin);

  if ((*is_comm = c == '#')) {
    /* move to the eol (eof if no eol) */
    while (!feof(stdin) && getc(stdin) != '\n');

    return true;
  }

  ungetc(c, stdin);
  *is_eof = getline(line_ptr, len, stdin) == -1;
  return line_ptr != NULL;
}
