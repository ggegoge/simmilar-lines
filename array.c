#include <stdlib.h>
#include <string.h>

#include "array.h"

size_t new_len(size_t curr_len)
{
  return ((size_t)curr_len + (curr_len >> 3) + 6) & ~(size_t)3;
}

void append(void* p, size_t width, void* new_el)
{
  DynArr* a = (DynArr*)p;
  ++a->used;

  if (a->used >= a->len) {
    a->len = new_len(a->len);
    a->val = realloc(a->val, a->len * width);

    if (!a->val)
      exit(1);
  }

  memcpy((char*)a->val + ((a->used - 1) * width), new_el, width);
}
