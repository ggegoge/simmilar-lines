#include <stdio.h>
#include <string.h>

#include "input.h"
#include "poly_parse.h"

/* POLYMORPHIC VARIANT */


size_t max(size_t a, size_t b)
{
  return (a >= b) ? a : b;
}

bool try_polymorphic(size_t* chunk_size, void* chunk, PLine* pline,
                     const char* s, const char* format,
                     bool(*add)(PLine*, void*))
{
  if (strlen(s) > *chunk_size) {
    *chunk_size = strlen(s);
    chunk = calloc(*chunk_size, 1);
  }

  return sscanf(s, format, chunk) && add(pline, chunk);

  if (sscanf(s, format, chunk))

    return add(pline, chunk);


  else
    return false;
}

bool add_parsed_int(PLine* pline, void* val)
{
  long long num = *(long long*)val;
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.val = (long long*)realloc(pline->ints.val,
                                          new_len(pline->ints.len) * sizeof(long long));

    if (pline->ints.val == NULL)
      return false;
  }

  pline->ints.val[pline->ints.used - 1] = num;
  return true;
}

bool add_parsed_float(PLine* pline, void* val)
{
  double num = *(double*)val;
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.val = (double*)realloc(pline->floats.val,
                                         new_len(pline->floats.len) * sizeof(double));

    if (pline->floats.val == NULL)
      return false;
  }
  
  pline->floats.val[pline->floats.used - 1] = num;
  return true;
}

bool add_parsed_string(PLine* pline, void* val)
{
  char* str = *(char** )val;
  size_t i;

  pline->nans.used++;

  for (i = 0; i < strlen(str); i++)
    if (str[i] < 33 || str[i] > 126) {
      fprintf(stderr, "ERROR LINE NUMBER %lu\n", pline->line_num);
      pline->nans.used--;
      return true;
    }

  if (pline->nans.used >= pline->nans.len) {
    pline->nans.val = (char**) realloc(pline->nans.val, new_len(pline->floats.len));

    if (pline->nans.val == NULL)
      return false;
  }

  pline->nans.val[pline->nans.used - 1] = str;
  return true;
}

bool try_parse(PLine* pline, const char* s)
{
  size_t chunk_size;
  void* chunk = malloc(CHUNK_SIZE);
  bool is_parsed;

  is_parsed =
    try_polymorphic(&chunk_size, chunk, pline, s, "%i", add_parsed_int) ||
    try_polymorphic(&chunk_size, chunk, pline, s, "%f", add_parsed_float) ||
    try_polymorphic(&chunk_size, chunk, pline, s, "%g", add_parsed_float) ||
    try_polymorphic(&chunk_size, chunk, pline, s, "%G", add_parsed_float) ||
    try_polymorphic(&chunk_size, chunk, pline, s, "%s", add_parsed_string);

  free(chunk);
  return is_parsed;
}

PLine parse(DLine dline)
{
  PLine pline;

  pline.line_num = dline.line_num;  
  
  /* strtok... */

  return pline;
}
