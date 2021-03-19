#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "group.h"


/* komparatory, zadeklaruję te nudy tutaj by nie była ich implementacja
 * pierwszą widzianą rzeczą. */
static int pline_cmp(const void*, const void*);
static int pword_cmp(const void*, const void*);
static int cmp_whole(Whole, Whole);
static int cmp_real(double, double);
static int cmp_size_t(const void*, const void*);
static int cmp_size_t_p(const void*, const void*);



/**
 * Normalizacja sparsowanych linijek z tablicy @plines długości @len,
 * poprzez ułożenie ichnich wielozbiorów w kolejności rosnącej. */
static void normalise(PLine* plines, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    qsort(plines[i].pwords.val, plines[i].pwords.used, sizeof(PWord), pword_cmp);
}

/**
 *  Procedura zakończenia przetwarzania obecnej @group-y i dodanie jej do tablicy
 *  wszystkich @groups */
void end_group(Group* group, Groups* all_groups)
{
  size_t* new_group;
  qsort(group->val, group->used, sizeof(size_t), cmp_size_t);
  new_group = (size_t*) malloc ((group->used + 1) * sizeof(size_t));

  if (!new_group)
    exit(1);

  /* zapisuję tam tabliczkę indeksów */
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
 * Znajdywanie podobnych sparsowanych (zał.: unormalizowanych) linijek w @plines
 * zakładając, że jest to tablica posortowana. */
static void find_similars(PLine* plines, size_t len)
{
  Groups all_groups;
  bool new_group = true;
  size_t i = 0;
  /* obecnie rozważana linijka i wzorzec obecnie rozwijanej grupy */
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

  /* edge case grupy skończonej mid file */
  if (group.used != 0)
    end_group(&group, &all_groups);

  /* output w kolejnosci nr wierszy */
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

  if (pl1.pwords.used != pl2.pwords.used)
    return (pl1.pwords.used < pl2.pwords.used) ? (-1) : 1;

  for (size_t i = 0; i < pl1.pwords.used; ++i) {
    if ((cmp = pword_cmp(&pl1.pwords.val[i], &pl2.pwords.val[i])))
      return cmp;
  }

  return 0;
}

static int pword_cmp(const void* p1, const void* p2)
{
  PWord pw1 = *(PWord*)p1;
  PWord pw2 = *(PWord*)p2;

  if (pw1.class == pw2.class) {
    switch (pw1.class) {
    case WHOLE:
      return cmp_whole(pw1.meaning.whole, pw2.meaning.whole);

    case REAL:
      return cmp_real(pw1.meaning.real, pw2.meaning.real);

    case NEITHER:
      return strcmp(pw1.meaning.nan, pw2.meaning.nan);

    default:
      return 0;
    }
  } else
    return pw1.class - pw2.class;
}

/**
 *  Funkcja porównujące dwie liczby @a i @b całkowite typu @Whole.
 *  Najpierw po znaku (- < +), później po wartości absolutnej. */
static int cmp_whole(Whole n1, Whole n2)
{
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

static int cmp_real(double n1, double n2)
{
  /* test odejmowawczy nie sprawdziłby się, albowiem inf - inf = ? */
  if (n1 != n2)
    return (n1 < n2) ? (-1) : 1;
  else
    return 0;
}

static int cmp_size_t(const void* a, const void* b)
{
  return *(size_t*)a - *(size_t*)b;
}

static int cmp_size_t_p(const void* a, const void* b)
{
  return (**(size_t**)a) - (**(size_t**)b);
}
