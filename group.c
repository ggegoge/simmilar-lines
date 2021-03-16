#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "group.h"


void write_groups(PText t)
{
  printf("\n\nwypisuje grupy:\n");
  normalise(t.val, t.used);
  qsort(t.val, t.used, sizeof(PLine), pline_cmp);
  find_similars(t.val, t.used);
}

/**
 * Normalizacja sparsowanych linijek z tablicy @plines długości @len. */
void normalise(PLine* plines, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    qsort(plines[i].wholes.val, plines[i].wholes.used, sizeof(Whole), cmp_whole);
    qsort(plines[i].reals.val, plines[i].reals.used, sizeof(double), cmp_real);
    qsort(plines[i].nans.val, plines[i].nans.used, sizeof(char*), cmp_nan);
  }
}

/**
 * Znajdywanie podobncyh sparsowanych (zał.: unormalizowanych) linijek w @plines
 * zakładając, że jest to tablica posortowana. @len to jej długość. */
void find_similars(PLine* plines, size_t len)
{
  Groups all_groups;
  bool new_group = true;
  size_t i = 0;
  PLine curr_line;
  PLine group_line;
  Group group;

  group.len = SMALL_ARRAY;
  group.used = 0;
  group.val = (size_t*)malloc(group.len * sizeof(size_t));

  if (!group.val)
    exit(1);

  all_groups.len = BIG_ARRAY;
  all_groups.used = 0;
  all_groups.val = (size_t**) malloc(all_groups.len * sizeof(size_t**));

  if (!all_groups.val)
    exit(1);

  while (i < len) {
    curr_line = plines[i];

    if (new_group) {
      group_line = curr_line;
      append(&group, sizeof(size_t), &curr_line.line_num);
      /* add_similar(&group, curr_line.line_num); */
      new_group = !new_group;
      ++i;
    } else if (pline_cmp(&curr_line, &group_line)) {
      end_group(&group, &all_groups);
      new_group = true;
    } else {
      append(&group, sizeof(size_t), &curr_line.line_num);
      /* add_similar(&group, curr_line.line_num); */
      ++i;
    }
  }

  if (group.used != 0)
    end_group(&group, &all_groups);

  qsort(all_groups.val, all_groups.used, sizeof(size_t*), cmp_size_t_p);
  print_all_groups(all_groups);
  free_groups(all_groups);
  free(group.val);
}


/**
 *  Procedura zakończenia przetwarzania obecnej grupy @group. */
void end_group(Group* group, Groups* all_groups)
{
  qsort(group->val, group->used, sizeof(size_t), cmp_size_t);

  add_group(*group, all_groups);
  group->used = 0;
}

void add_group(Group group, Groups* all_groups)
{
  size_t* new_group = (size_t*) malloc((group.used + 1) * sizeof(size_t));

  if (!new_group)
    exit(1);

  memcpy(new_group, group.val, sizeof(size_t) * group.used);
  /* brak wiersza zerowego -- mogę tak markować koniec */
  new_group[group.used] = 0;
  append(all_groups, sizeof(size_t*), &new_group);
}

void print_all_groups(Groups all_groups)
{
  size_t i, j;

  for (i = 0; i < all_groups.used; ++i) {
    j = 0;

    while (all_groups.val[i][j + 1] != 0) {
      printf("%lu ", all_groups.val[i][j]);
      ++j;
    }

    printf("%lu\n", all_groups.val[i][j]);
  }
}

void free_groups(Groups all_groups)
{
  for (size_t i = 0; i < all_groups.used; ++i)
    free(all_groups.val[i]);

  free(all_groups.val);
}

/**
 *  Funkcja porównująca wiersze @l1 oraz @l2. Jak wszystkie funkcje porównujące
 *  w tym pliku, zwraca (-1) gdy @l1 < @l2, 0 gdy @l1 = @l2 i 1 gdy @l1 > @l2. */
int pline_cmp(const void* l1, const void* l2)
{
  PLine pl1 = *(PLine*)l1;
  PLine pl2 = *(PLine*)l2;
  int cmp;

  if ((cmp = arrays_cmp(pl1.wholes.val, pl1.wholes.used, pl2.wholes.val,
                        pl2.wholes.used, sizeof(Whole), cmp_whole)))
    return cmp;

  if ((cmp = arrays_cmp(pl1.reals.val, pl1.reals.used, pl2.reals.val,
                        pl2.reals.used, sizeof(double), cmp_real)))
    return cmp;

  if ((cmp = arrays_cmp(pl1.nans.val, pl1.nans.used, pl2.nans.val,
                        pl2.nans.used, sizeof(char*), cmp_nan)))
    return cmp;


  return 0;
}

/**
 *  Polimorficzna funkcja porównująca tablice @a1 i @a2 o długościach
 *  (odpowiednio) @len1 i @len2 oraz o polach wielkości @size. Do porównania
 *  poszczególnych pól korzysta z funkcji @compare. */
int arrays_cmp(const void* a1, size_t len1, const void* a2, size_t len2,
               size_t size, int(*compare)(const void*, const void*))
{
  int cmp;

  if (len1 != len2)
    return (len1 < len2) ? (-1) : 1;

  for (size_t i = 0; i < len1; ++i) {
    if ((cmp = compare((char*)a1 + (i * size), (char*)a2 + (i * size))))
      return cmp;
  }

  return 0;
}

/**
 *  Funkcja porównujące dwie liczby @a i @b całkowite typu @Whole. */
int cmp_whole(const void* a, const void* b)
{
  Whole n1 = *(Whole*)a;
  Whole n2 = *(Whole*)b;

  if (n1.sign == n2.sign) {
    if (n1.sign == MINUS) {

      if (n1.abs != n2.abs)
        return (n1.abs < n2.abs) ? 1 : (-1);
      else
        return 0;

    } else {

      if (n1.abs != n2.abs)
        return (n1.abs < n2.abs) ? (-1) : 1;
      else
        return 0;
    }
  } else
    return (n1.sign == MINUS && n2.sign == PLUS) ? (-1) : 1;
}

/**
 * Funkcja służąca porównaniu liczb rzeczywistych @x i @y typu @double. */
int cmp_real(const void* x, const void* y)
{
  double n1 = *(double*)x;
  double n2 = *(double*)y;

  if (n1 != n2)
    return (n1 < n2) ? (-1) : 1;
  else
    return 0;
}

/**
 *  Porównywanie nieliczb. De facto wrapper wokół stdlibowej strcmp. Dostaje
 *  wskaźniki na stringi @sp1 i @sp2, które porównuje. */
int cmp_nan(const void* sp1, const void* sp2)
{
  return strcmp(*(char**)sp1, *(char**)sp2);
}

/**
 * Porównanie zmiennych typu size_t. */
int cmp_size_t(const void* a, const void* b)
{
  return *(size_t*)a - *(size_t*)b;
}

int cmp_size_t_p(const void* a, const void* b)
{
  return **(size_t**)a -** (size_t**)b;
}
