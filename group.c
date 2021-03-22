#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "group.h"


/* komparatory, zadeklaruję te nudy tutaj by nie była ich implementacja
 * pierwszą widzianą rzeczą. Każda nazwa wskazuje na typ porównywanych rzeczy
 * (w tym size_t_p oznacza wskaźnik [tzw pointer] na size_t).*/
static int cmp_pline(const void*, const void*);
static int cmp_pword(const void*, const void*);
static int cmp_whole(Whole, Whole);
static int cmp_real(double, double);
static int cmp_size_t(const void*, const void*);
static int cmp_size_t_p(const void*, const void*);

/**
 * Normalizacja sparsowanych linijek z tablicy @plines długości @len,
 * poprzez ułożenie ichnich wielozbiorów w kolejności rosnącej. */
static void normalise(ParsedLine* plines, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    qsort(plines[i].pwords.val, plines[i].pwords.used, sizeof(ParsedWord),
          cmp_pword);
}

/**
 *  Procedura zakończenia przetwarzania obecnej @group-y i dodanie jej do tablicy
 *  wszystkich @groups */
void end_group(Group* group, Groups* all_groups)
{
  size_t* new_group;
  /* długość tablicy indeksów musi być zwiększona o 1 ponieważ na ich końcu
   * zapisywane zostaje 0. względu na numeracje wierszy od 1 brak zerowego
   * wiersza, tym samym jest 0 dobrym wyznacznikiem końca jednej grupy w tablicy. */
  size_t group_len = (group->used + 1) * sizeof(size_t);
  qsort(group->val, group->used, sizeof(size_t), cmp_size_t);
  new_group = (size_t*) malloc(group_len);

  if (!new_group)
    exit(1);

  /* zapisuję tam tabliczkę indeksów */
  memcpy(new_group, group->val, sizeof(size_t) * group->used);
  /* wspomniane 0 jako sygnalizator końca tablicy liczb >0 */
  new_group[group->used] = 0;
  array_append(all_groups, sizeof(size_t*), &new_group);
  group->used = 0;
}

/**
 * Właściwe wypisanie wszystkich grup.  */
void print_all_groups(Groups all_groups)
{
  size_t i, j;

  for (i = 0; i < all_groups.used; ++i) {
    /* pętla służy wypisaniu wszystkich podobnych wierszy jednej grupy, prócz
     * ostatniego (po którym następuje 0), ponieważ po nim nie nastąpi spacja */
    for (j = 0; all_groups.val[i][j + 1] != 0; ++j)
      printf("%lu ", all_groups.val[i][j]);

    /* ostatni element ma \n, nie spację */
    printf("%lu\n", all_groups.val[i][j]);
  }
}

/**
 * Znajdywanie podobnych sparsowanych (zał.: unormalizowanych) linijek w @plines
 * zakładając, że jest to tablica posortowana. Po ich znalezieniu wywołuje
 * print_all_groups celem wypisania tych grup na stdout w sposob opisany w treści
 * zadania. */
static void find_similars(ParsedLine* plines, size_t len)
{
  Groups all_groups;
  bool new_group = true;
  size_t i = 0;
  /* obecnie rozważana linijka i wzorzec obecnie rozwijanej grupy */
  ParsedLine curr_line, group_line;
  Group group;

  if (len == 0)
    return;

  /* Zakładam (może mylnie), że częściej zdarza się wiele grup rozczłonkowanych
   * niżli jedna wielka, zatem domyslnie przeznaczam więcej miejsca na tablicę
   * wszystkich grup niż pojedynczą grupę. */
  array_init(&group, sizeof(size_t), SMALL_ARRAY);
  array_init(&all_groups, sizeof(size_t**), BIG_ARRAY);

  while (i < len) {
    curr_line = plines[i];

    if (new_group) {
      /* brak wzorca do porównania - nówka grupka */
      group_line = curr_line;
      array_append(&group, sizeof(size_t), &curr_line.line_num);
      new_group = !new_group;
      ++i;
    } else if (cmp_pline(&curr_line, &group_line)) {
      /* linijka różna od wzorca */
      end_group(&group, &all_groups);
      new_group = true;
    } else {
      /* linijki są równe */
      array_append(&group, sizeof(size_t), &curr_line.line_num);
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

void write_groups(ParsedText t)
{
  normalise(t.val, t.used);
  /* sortuję wszystkie linijki aby potem łatwo znaleźć duplikaty */
  qsort(t.val, t.used, sizeof(ParsedLine), cmp_pline);
  find_similars(t.val, t.used);
}

/* komparatory. Zasadniczo:
 *   cmp(a, b) = if a < b then -1 else if a > b then 1 else 0,
 * Porównując rzecz złożoną (np tablicę, struktury) pierwsza różnica decyduje
 * o porządku między elementami -- quasi leksykograficznie porządkuję. */

static int cmp_pline(const void* l1, const void* l2)
{
  ParsedLine pl1 = *(ParsedLine*)l1;
  ParsedLine pl2 = *(ParsedLine*)l2;
  int cmp;

  if (pl1.pwords.used != pl2.pwords.used)
    return (pl1.pwords.used < pl2.pwords.used) ? -1 : 1;

  for (size_t i = 0; i < pl1.pwords.used; ++i) {
    /* pierwsza różnica decyduje ponieważ zał, że linie są posortowane */
    if ((cmp = cmp_pword(&pl1.pwords.val[i], &pl2.pwords.val[i])))
      return cmp;
  }

  return 0;
}

static int cmp_pword(const void* p1, const void* p2)
{
  ParsedWord pw1 = *(ParsedWord*)p1;
  ParsedWord pw2 = *(ParsedWord*)p2;

  if (pw1.class == pw2.class) {
    switch (pw1.class) {
    case WHOLE:
      return cmp_whole(pw1.whole, pw2.whole);

    case REAL:
      return cmp_real(pw1.real, pw2.real);

    case NEITHER:
      return strcmp(pw1.nan, pw2.nan);

    default:
      return 0;
    }
  } else
    return pw1.class - pw2.class;
}

/**
 *  Funkcja porównujące dwie liczby @a i @b całkowite typu @Whole.
 *  Najpierw po znaku (- < +), później po wartości absolutnej (w przypadku znaku
 *  MINUS porządek się obraca). */
static int cmp_whole(Whole n1, Whole n2)
{
  if (n1.sign == n2.sign) {
    if (n1.sign == MINUS)
      return n1.abs != n2.abs ? (n1.abs < n2.abs ? 1 : -1) : 0;
    else
      return n1.abs != n2.abs ? (n1.abs < n2.abs ? -1 : 1) : 0;
  } else
    return (n1.sign == MINUS && n2.sign == PLUS) ? -1 : 1;
}

static int cmp_real(double n1, double n2)
{
  return n1 != n2 ? (n1 < n2 ? -1 : 1) : 0;
}

static int cmp_size_t(const void* a, const void* b)
{
  size_t n1 = *(size_t*)a;
  size_t n2 = *(size_t*)b;
  return n1 != n2 ? (n1 < n2 ? -1 : 1) : 0;
}

static int cmp_size_t_p(const void* a, const void* b)
{
  size_t n1 = **(size_t**)a;
  size_t n2 = **(size_t**)b;
  return n1 != n2 ? (n1 < n2 ? -1 : 1) : 0;
}
