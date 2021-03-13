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
  printf("parsing line #%lu ", pline.line_num);

  for (i = 0; i < dline.used; i++)
    printf("%c", dline.val[i]);
  
  return pline;
}

PText init_ptext()
{
  PText pt;
  pt.len = INIT_ARR_SIZE;
  pt.used = 0;
  pt.val = (PLine*) malloc(INIT_ARR_SIZE * sizeof(PLine));
  return pt;
}

void free_text(PText text)
{
  if (text.val != NULL) {
    free(text.val);
    text.val = NULL;
  }
}

bool add_parsed_line(PText* ptext, PLine pline)
{
  ptext->used++;

  if (ptext->used >= ptext->len) {
    ptext->val = (PLine*) realloc(ptext->val, new_len(ptext->len));

    if (ptext->val == NULL)
      return false;
  }

  ptext->val[ptext->used - 1] = pline;
  return true;
}

bool add_parsed_float(PLine* pline, double num)
{
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.val = (double*) realloc(pline->floats.val,
                                          pline->floats.len * sizeof(double));

    if (pline->floats.val == NULL)
      return false;
  }

  pline->floats.val[pline->floats.used - 1] = num;
  return true;
}

bool add_parsed_int(PLine* pline, long long num)
{
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.val = (long long*) realloc(pline->ints.val,
                                           pline->ints.len * sizeof(long long));

    if (pline->ints.val == NULL)
      return false;
  }

  pline->ints.val[pline->ints.used - 1] = num;
  return true;
}

bool add_parsed_string(PLine* pline, char* str)
{
  pline->nans.used++;

  if (pline->nans.used >= pline->nans.len) {
    pline->nans.val = (char**) realloc(pline->nans.val, new_len(pline->floats.len));

    if (pline->nans.val == NULL)
      return false;
  }

  pline->nans.val[pline->nans.used - 1] = str;
  return true;
}




