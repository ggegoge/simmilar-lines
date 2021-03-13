#include <stdio.h>
#include <string.h>

#include "poly_parse.h"

/* POLYMORPHIC VARIANT */



size_t max(size_t a, size_t b)
{
  return (a >= b) ? a : b;
}

int try_polymorphic(PLine* pline, const char* s, const char* format,
                    void(*add)(PLine*, void*))
{
  void* val = malloc(max(strlen(s), CHUNK_SIZE));

  if (sscanf(s, format, val)) {
    add(pline, val);
    return 1;
  } else
    return 0;
}

void add_parsed_int(PLine* pline, void* val)
{
  long long num = *(long long*)val;
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.val = (long long*)realloc(pline->ints.val,
                                          pline->ints.len * sizeof(long long));

    if (!pline->ints.val)
      fprintf(stderr, "REALLOC FAILURE");
  }

  pline->ints.val[pline->ints.used - 1] = num;
}

void add_parsed_float(PLine* pline, void* val)
{
  double num = *(double*)val;
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.val = (double*)realloc(pline->floats.val,
                                         pline->floats.len * sizeof(double));

    if (!pline->floats.val)
      fprintf(stderr, "REALLOC FAILURE");
  }

  pline->floats.val[pline->floats.used - 1] = num;
}

void add_parsed_string(PLine* pline, void* val)
{
  char* str = *(char** )val;
  pline->nans.used++;
  /* etc */
}

int try_parse(PLine* pline, const char* s)
{
  if (try_polymorphic(pline, s, "%i", add_parsed_int) ||
      try_polymorphic(pline, s, "%f", add_parsed_float) ||
      try_polymorphic(pline, s, "%g", add_parsed_float) ||
      try_polymorphic(pline, s, "%G", add_parsed_float) ||
      try_polymorphic(pline, s, "%s", add_parsed_string))
    return 1;
  else
    return 0;
}
