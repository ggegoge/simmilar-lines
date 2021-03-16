#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "array.h"
#include "input.h"
#include "parse.h"
#include "group.h"
#include "main.h"


int main(void)
{
  PText text = init_ptext();
  read_text(&text);
  write_groups(text);
  free_text(text);

  return 0;
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

      if (pline.well_formed)
        append(ptext, sizeof(PLine), &pline);
      else
        free_line(pline);
    }

    ++line_num;
  }

  free(line);
}
