#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "group.h"


/* komparatory, zadeklaruję te nudy tutaj by nie była ich implementacja
 * pierwszą widzianą rzeczą. */
static int arrays_cmp(const void* a1, size_t len1, const void* a2, size_t len2,
                      size_t width, int(*compare)(const void*, const void*));
static int pline_cmp(const void*, const void*);
static int cmp_whole(const void*, const void*);
static int cmp_real(const void*, const void*);
static int cmp_nan(const void*, const void*);
static int cmp_size_t(const void*, const void*);
static int cmp_size_t_p(const void*, const void*);


/**
 * Normalizacja sparsowanych linijek z tablicy @plines długości @len. */
static void normalise(PLine* plines, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    qsort(plines[i].wholes.val, plines[i].wholes.used, sizeof(Whole), cmp_whole);
    qsort(plines[i].reals.val, plines[i].reals.used, sizeof(double), cmp_real);
    qsort(plines[i].nans.val, plines[i].nans.used, sizeof(char*), cmp_nan);
  }
}

/**
 *  Procedura zakończenia przetwarzania obecnej grupy i dodanie jej do tablicy
 *  wszystkich grup. */
void end_group(Group* group, Groups* all_groups)
{
  size_t* new_group;
  qsort(group->val, group->used, sizeof(size_t), cmp_size_t);
  new_group = (size_t*) malloc ((group->used + 1) * sizeof(size_t));

  if (!new_group)
    exit(1);

  memcpy(new_group, group->val, sizeof(size_t) * group->used);
  new_group[group->used] = 0;
  append(all_groups, sizeof(size_t*), &new_group);
  group->used = 0;
}

/**
 * Właściwe wypisanie wszystkich grup.  */
void print_all_groups(Groups all_groups)
{
  size_t i, j;

  for (i = 0; i < all_groups.used; ++i) {
    for (j = 0; all_groups.val[i][j + 1] != 0; ++j)
      printf("%lu ", all_groups.val[i][j]);

    /* ostatni element ma \n, nie spację */
    printf("%lu\n", all_groups.val[i][j]);
  }
}

/**
 * Znajdywanie podobncyh sparsowanych (zał.: unormalizowanych) linijek w @plines
 * zakładając, że jest to tablica posortowana. @len to jej długość. */
static void find_similars(PLine* plines, size_t len)
{
  Groups all_groups;
  bool new_group = true;
  size_t i = 0;
  PLine curr_line, group_line;
  Group group;
  
  if (len == 0)
    return;
  init(&group, sizeof(size_t), SMALL_ARRAY);
  init(&all_groups, sizeof(size_t**), BIG_ARRAY);

  while (i < len) {
    curr_line = plines[i];

    if (new_group) {
      /* brak wzorca do porównania - nówka grupka */
      group_line = curr_line;
      append(&group, sizeof(size_t), &curr_line.line_num);
      new_group = !new_group;
      ++i;
    } else if (pline_cmp(&curr_line, &group_line)) {
      /* linijka różna od wzorca */
      end_group(&group, &all_groups);
      new_group = true;
    } else {
      /* linijki są równe */
      append(&group, sizeof(size_t), &curr_line.line_num);
      ++i;
    }
  }
  /* grupa skończona mid file */
  if (group.used != 0)
    end_group(&group, &all_groups);

  qsort(all_groups.val, all_groups.used, sizeof(size_t*), cmp_size_t_p);
  print_all_groups(all_groups);

  for (i = 0; i < all_groups.used; ++i)
    free(all_groups.val[i]);

  free(all_groups.val);
  free(group.val);
}


void write_groups(PText t)
{
  normalise(t.val, t.used);
  /* sortuję wszystkie linijki aby potem łatwo znaleźć duplikaty */
  qsort(t.val, t.used, sizeof(PLine), pline_cmp);
  find_similars(t.val, t.used);
}


/* komparatory. Zasadniczo:
 *   cmp(a, b) = if a < b then -1 else if a > b then 1 else 0,
 * prócz porównań na zwykłych liczbach, gdzie prościej jest zwrócić różnicę.  */

static int pline_cmp(const void* l1, const void* l2)
{
  PLine pl1 = *(PLine*)l1;
  PLine pl2 = *(PLine*)l2;
  int cmp;

  /* pierwsza nierówność lub równość */
  if ((cmp = arrays_cmp(pl1.wholes.val, pl1.wholes.used, pl2.wholes.val,
                        pl2.wholes.used, sizeof(Whole), cmp_whole)) ||
      (cmp = arrays_cmp(pl1.reals.val, pl1.reals.used, pl2.reals.val,
                        pl2.reals.used, sizeof(double), cmp_real)) ||
      (cmp = arrays_cmp(pl1.nans.val, pl1.nans.used, pl2.nans.val,
                        pl2.nans.used, sizeof(char*), cmp_nan)))
    return cmp;
  else
    return 0;
}

/**
 *  Polimorficzna funkcja porównująca tablice @a1 i @a2 o długościach
 *  (odpowiednio) @len1 i @len2 oraz o polach wielkości @size. Do porównania
 *  poszczególnych pól korzysta z funkcji @compare. */
static int arrays_cmp(const void* a1, size_t len1, const void* a2, size_t len2,
                      size_t width, int(*compare)(const void*, const void*))
{
  int cmp;

  if (len1 != len2)
    return (len1 < len2) ? (-1) : 1;

  for (size_t i = 0; i < len1; ++i)
    if ((cmp = compare((char*)a1 + (i * width), (char*)a2 + (i * width))))
      return cmp;

  return 0;
}


/**
 *  Funkcja porównujące dwie liczby @a i @b całkowite typu @Whole.
 *  Najpierw po znaku (- < +), później po wartości absolutnej. */
static int cmp_whole(const void* a, const void* b)
{
  Whole n1 = *(Whole*)a;
  Whole n2 = *(Whole*)b;

  /* porównanie co do znaku i odpowiedniej wartości absolutnej.
   * test odejmowawczy odpada, ponieważ może przebić się zakres rozwalić jakoś.
   * Przeto ifologia */
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

static int cmp_real(const void* x, const void* y)
{
  /* test odejmowawczy nie sprawdziłby się, albowiem inf - inf = ? */
  double n1 = *(double*)x;
  double n2 = *(double*)y;

  if (n1 != n2)
    return (n1 < n2) ? (-1) : 1;
  else
    return 0;
}

static int cmp_nan(const void* sp1, const void* sp2)
{
  return strcmp(*(char**)sp1, *(char**)sp2);
}

static int cmp_size_t(const void* a, const void* b)
{
  return *(size_t*)a - *(size_t*)b;
}

static int cmp_size_t_p(const void* a, const void* b)
{
  return (**(size_t**)a) - (**(size_t**)b);
}
