#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "input.h"
#include "parse.h"
#include "group.h"


/* int read_text(PText*); */
bool read_text_getv(PText*);

int main(void)
{
  PText text = init_ptext();

  printf("Wpisz rzeczy:\n");

  /* if (read_text(&text) != 0)
   *   return 1;
   * 
   * if (write_groups(text) != 1)
   *   return -1;
   * 
   * free_text(text); */

  if (!read_text_getv(&text) || !write_groups(text))
    return 1;

  free_text(text);
  
  return 0;
}


bool read_text_getv(PText* ptext)
{
  bool read_code;    /* check if no memory problems on the way */
  size_t lin_num = 1;

  size_t len = 0;
  char* line = NULL;
  bool is_comm = false, is_eof = false;

  PLine pline;


  while (!feof(stdin) && !is_eof) {
    read_code = read_line_getv(&line, &len, &is_eof, &is_comm);

    if (!read_code)
      return false;

    if (!is_comm) {
      pline = parse_getv(line, lin_num);

      if (!pline.alloc_stat)
        return false;

      if (pline.well_formed)
        add_parsed_line(ptext, pline);
    }

    ++lin_num;
  }

  free(line);
  return true;
}

/* int read_text(PText* ptext)
 * {
 *   int read_code, parse_code;    /\* check if no memory problems on the way *\/
 *   size_t lin_num = 1;
 *   DLine line = init_line(INIT_ARR_SIZE, lin_num);
 *   PLine pline;
 * 
 *   if (line.initialised == -1)
 *     return -1;
 * 
 *   if (line.val == NULL)
 *     return -1;
 * 
 *   while (!feof(stdin)) {
 *     read_code = read_line(lin_num, &line);
 * 
 *     if (read_code == -1)
 *       return read_code;
 * 
 *     printf("line.commentary = %d\n", line.commentary);
 * 
 *     if (line.commentary != 1) {
 *       pline = parse(line);
 *       parse_code = pline.alloc_stat;
 * 
 *       if (pline.well_formed)
 *         add_parsed_line(ptext, pline);
 * 
 *       if (parse_code == -1)
 *         return -1;
 *     }
 * 
 *     line.used = 0;
 *     line.commentary = 0;
 *     ++lin_num;
 *   }
 * 
 *   free(line.val);
 *   return 1;
 * } */
