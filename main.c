#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "input.h"
#include "parse.h"
#include "group.h"


void read_text(PText*);

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
  
    if (!is_comm) {
      pline = parseln(line, line_num);
  
      if (pline.well_formed)
        add_parsed_line(ptext, pline);
    }
  
    ++line_num;
  }
  
  free(line);
}
