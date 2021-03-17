#ifndef ARRAY_H
#define ARRAY_H


#include <stdlib.h>
#include <string.h>

/* ogólne narzędzia do wygodnego operowania na tablicach dynamicznych. */

#define SMALL_ARRAY 8
#define BIG_ARRAY 16

/**
 * "Nadklasa" dynamicznych tablic tj wzorzec dla innych. Powinny różnić się
 * jedynie typem pola @val (tj. właściwą tablicą). Pola @used i @len to
 * odpowiednio liczba zajętych bloków tablicy @val oraz jej faktyczna długość
 * w pamięci (albowiem alokuję ponad stan). */
typedef struct dyn_anything {
  size_t used, len;
  void* val;
} DynArr;

/* co prawda nie eksportuję powyższej struktury, ale jako wzorzec niech będzie
 * wyeksponowana tutaj. */

/**
 * Polimorficzne dodanie elementu pod adresem @new_el do dynamicznej tablicy
 * wskazywanej przez @p, o elementach wielkości @width */
void append(void* p, size_t width, void* new_el);

/**
 *  Inicjalizacja tablicy wskazywanej przez @p o elementach wielkości @width
 *  na daną długość @len. */
void init(void* p, size_t width, size_t len);
  
#endif /* ARRAY_H */
