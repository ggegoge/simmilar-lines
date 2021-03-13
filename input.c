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

int append_to_line(DLine* line, char new_char)
{
  line->used++;

  if (line->used >= line->len) {
    line->len = new_len(line->len);
    line->val = (char*)realloc(line->val, line->len * sizeof(char));

    if (!line->val) {
      fprintf(stderr, "REALLOC FAILURE\n");
      return -1;
    }
  }

  line->val[line->used - 1] = new_char;
  return 0;
}

DLine init_line(size_t init_len, size_t line_num)
{
  DLine line;
  line.line_num = line_num;
  line.used = 0;
  line.len = init_len;
  line.val = (char*)malloc(init_len * sizeof(char));
  line.commentary = 0;

  if (!line.val)
    line.initialised = -1;

  return line;
}

int read_line(size_t line_num, DLine* line)
{
  int alloc_status = 0;
  char c;

  line->line_num = line_num;

  if (scanf("%c", &c) != 1)
    return alloc_status;

  if (c == '#') {
    printf("to jest kom\n");
    line->commentary = 1;

    while (scanf("%c", &c) == 1 && c != '\n');

    return alloc_status;
  } else
    printf("fisrt char is: %c\n", c);

  do {
    alloc_status = append_to_line(line, c);
  } while (alloc_status == 0 && c != '\n' && scanf("%c", &c) == 1 &&
           !feof(stdin));

  return alloc_status;
}


int read_text(PText* ptext)
{
  int read_code, parse_code;    /* check if no memory problems on the way */
  size_t lin_num = 1;
  DLine line = init_line(INIT_ARR_SIZE, lin_num);
  PLine pline;

  if (line.initialised == -1)
    return -1;

  if (line.val == NULL)
    return -1;

  while (!feof(stdin)) {
    read_code = read_line(lin_num, &line);

    if (read_code == -1)
      return read_code;

    printf("line.commentary = %d\n", line.commentary);

    if (line.commentary != 1) {
      pline = parse(line);
      parse_code = pline.alloc_stat;

      if (pline.well_formed)
        add_parsed_line(ptext, pline);

      if (parse_code == -1)
        return -1;
    }

    line.used = 0;
    line.commentary = 0;
    ++lin_num;
  }

  free(line.val);
  return 1;
}
