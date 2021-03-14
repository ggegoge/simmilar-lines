#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "input.h"

PLine parse(char* line, size_t line_num)
{
  size_t i;
  PLine pline;

  pline.line_num = line_num;
  pline.well_formed = true;
  printf("parsing line #%lu ", pline.line_num);

  for (i = 0; i < strlen(line); ++i)
    printf("%c", line[i]);

  return pline;
}

PText init_ptext()
{
  PText ptext;
  ptext.len = INIT_ARR_SIZE;
  ptext.used = 0;
  ptext.val = (PLine*) malloc(INIT_ARR_SIZE * sizeof(PLine));
  return ptext;
}

void free_text(PText text)
{
  if (!text.val) {
    free(text.val);
    text.val = NULL;
  }
}

void add_parsed_line(PText* ptext, PLine pline)
{
  ++ptext->used;

  if (ptext->used >= ptext->len) {
    ptext->val = (PLine*) realloc(ptext->val, new_len(ptext->len) * sizeof(PLine));

    if (!ptext->val)
      exit(1);
  }

  ptext->val[ptext->used - 1] = pline;
}

void add_parsed_float(PLine* pline, double num)
{
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.val = (double*) realloc(pline->floats.val,
                                          pline->floats.len * sizeof(double));

    if (!pline->floats.val)
      exit(1);
  }

  pline->floats.val[pline->floats.used - 1] = num;
}

void add_parsed_int(PLine* pline, long long num)
{
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.val = (long long*) realloc(pline->ints.val,
                                           pline->ints.len * sizeof(long long));

    if (!pline->ints.val)
      exit(1);
  }

  pline->ints.val[pline->ints.used - 1] = num;
}

void add_parsed_string(PLine* pline, char* str)
{
  pline->nans.used++;

  if (pline->nans.used >= pline->nans.len) {
    pline->nans.val = (char**) realloc(pline->nans.val, new_len(pline->floats.len));

    if (!pline->nans.val)
      exit(1);
  }

  pline->nans.val[pline->nans.used - 1] = str;
}




