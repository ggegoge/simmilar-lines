#include <stdio.h>

#include "parse.h"
#include "input.h"

PLine parse(DLine dline)
{
  PLine pline;
  size_t i;

  pline.alloc_stat = 1;
  pline.well_formed = 1;
  pline.line_num = dline.line_num;
  printf("parsing line #%lu", pline.line_num);

  for (i = 0; i < dline.used; i++)
    printf("%c", dline.val[i]);

  return pline;
}

void free_text(PText text)
{
  /* free(text.val); */
  return;
}

int add_parsed_line(PText* ptext, PLine pline)
{
  return 1;
}

int add_parsed_float(PLine* pline, double num)
{
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.val = (double*) realloc(pline->floats.val,
                                          pline->floats.len * sizeof(double));

    if (pline->floats.val == NULL)
      return -1;
  }

  pline->floats.val[pline->floats.used - 1] = num;
  return 1;
}

int add_parsed_int(PLine* pline, long long num)
{
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.val = (long long*) realloc(pline->ints.val,
                                           pline->ints.len * sizeof(long long));

    if (pline->ints.val == NULL)
      return -1;
  }

  pline->ints.val[pline->ints.used - 1] = num;
  return 1;
}

int add_parsed_string(PLine* pline, char* str)
{
  return 1;
}



int try_int_format(PLine* pline, const char* s, const char* format)
{
  unsigned long long num;

  if (sscanf(s, format, &num) == 1) {
    add_parsed_int(pline, (long long) num);
    return 1;
  } else
    return 0;
}

int try_int(PLine* pline, const char* s)
{
  if (try_int_format(pline, s, "%lli"))
    return 1;
  else
    return 0;
}

int try_float_format(PLine* pline, const char* s, const char* format)
{
  double num;

  if (sscanf(s, format, &num)) {
    add_parsed_float(pline, (double)num);
    return 1;
  } else
    return 0;
}


int try_float(PLine* pline, const char* s)
{
  if (try_float_format(pline, s, "%lg") || try_float_format(pline, s, "%lG") ||
      try_float_format(pline, s, "%llf"))
    return 1;
  else
    return 0;
}

int try_string();


