#ifndef ARRAY_H
#define ARRAY_H


#include <stdlib.h>
#include <string.h>

/* moduł arrays -- tutaj schowam też rzeczy, które nie są właściwe żadnemu z modułów */

#define SMALL_ARRAY 4
#define BIG_ARRAY 16
#define ARR_EXPAND 1.25


/**
 * Nowa długość dla tablicy w oparciu o @curr_len. */
size_t new_len(size_t curr_len);

/**
 * Nadklasa dynamicznych tablic. */
typedef struct dyn_anything {
  size_t used, len;
  void* val;
} DynArr;

/**
 * Polimorficzne dodanie elementu pod adresem @new_el do dynamicznej tablicy
 * wskazywanej przez @p, o elementach wielkości @width */
void append(void* p, size_t width, void* new_el);

#endif /* ARRAY_H */
