#ifndef COMPARE_H
#define COMPARE_H

#include "parse.h"

/**
 * Komparatory. Każda nazwa wskazuje na typ porównywanych rzeczy (w tym size_t_p
 * oznacza wskaźnik [tzw pointer] na size_t). Dla funkcji qsort. */

int cmp_pword(const void* p1, const void* p2);

int cmp_pline(const void* l1, const void* l2);

/**
 * Wrapper na cmp_pline -- w przypadku równości dwóch linijek używa ichnich
 * indeksów jako swoisty `fallback' aby rozsądzić porządek. W cywilizowanych
 * językach użyłbym jakiejś małej lambdki, jest to zaledwie zabieg wynikający
 * z chęci stabilizacji sortu */
int cmp_pline_stable(const void* l1, const void* l2);

/**
 * Porządkowanie posortowanych tablic ich względem elementu minimalnego. */
int cmp_size_t_p(const void* a, const void* b);

#endif /* COMPARE_H */
