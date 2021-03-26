#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "compare.h"
#include "group.h"

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
  array_init(&group, sizeof(size_t), SMALL_ARRAY_LENGTH);
  array_init(&all_groups, sizeof(size_t**), BIG_ARRAY_LENGTH);

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
  qsort(t.val, t.used, sizeof(ParsedLine), cmp_pline_stable);
  find_similars(t.val, t.used);
}
