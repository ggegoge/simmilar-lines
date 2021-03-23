#ifndef COMPARE_H
#define COMPARE_H

#include "parse.h"

/**
 * Komparatory. Każda nazwa wskazuje na typ porównywanych rzeczy
 * (w tym size_t_p oznacza wskaźnik [tzw pointer] na size_t).
 * Część służy porównaniu na rzecz qsortu, zatem dostają pointer, a część po
 * prostu porównuje dwa obiekty danego typu. */


int cmp_pline(const void*, const void*);
int cmp_pword(const void*, const void*);
int cmp_whole(Whole, Whole);
int cmp_real(double, double);
int cmp_size_t(const void*, const void*);
int cmp_size_t_p(const void*, const void*);

#endif /* COMPARE_H */
