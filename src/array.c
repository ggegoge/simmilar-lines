#include <stdlib.h>
#include <string.h>

#include "array.h"

/* współczynnik zwiększania się tablic dynamicznych */
#define ARRAY_RESIZE 2

/**
 * Nowa poszerzona długość dla tablicy (alokacja z zapasem). */
static inline size_t new_len(size_t new_len)
{
  return new_len * ARRAY_RESIZE;
}

void array_append(void* p, size_t width, void* new_el)
{
  DynArr* a = (DynArr*)p;
  ++a->used;

  if (a->used >= a->len) {
    a->len = new_len(a->used);
    a->val = realloc(a->val, a->len * width);

    if (!a->val)
      exit(1);
  }

  /* przelewam dane spod adresu nowego elementu pod ostatni indeks */
  memcpy((char*)a->val + (a->used - 1) * width, new_el, width);
}

void array_init(void* p, size_t width, size_t len)
{
  DynArr* a = (DynArr*)p;
  a->used = 0;
  a->len = len;

  /* inicjalizacja pustej tablicy */
  if (a->len == 0) {
    a->val = NULL;
    return;
  }

  a->val = malloc(a->len * width);

  if (!a->val)
    exit(1);
}
