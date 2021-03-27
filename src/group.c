#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"
#include "parse.h"
#include "compare.h"
#include "group.h"

/**
 * Pojedyncza grupa podobnych wierszy, tablica dynamiczna rodem z array.h,
 * znaczenia pól used i len ibidem. W swoich trzewiach val trzyma indeksy
 * wszystkich podobnych wierszy danej grupy. */
typedef struct Group {
  size_t used, len;
  size_t* val;
} Group;

/**
 * Kolejna tablica dynamiczna w wiadomym stylu. Trzyma wiele grup, każdą jako
 * tablicę indeksów. Końce wewnętrznych tablic znakuje poprzez 0 analogicznie
 * jak \0 w stringach. Jest to właściwe, ponieważ wiersze numerujemy od 1 */
typedef struct Groups {
  size_t used, len;
  size_t** val;
} Groups;

/**
 * Normalizacja sparsowanych linijek poprzez ułożenie ichnich wielozbiorów
 * w kolejności rosnącej tak by dwa identyczne podzbiory były reprezentowane
 * przez takie same tablice słów */
static void normalise(ParsedLine* plines, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    qsort(plines[i].pwords.val, plines[i].pwords.used, sizeof(ParsedWord),
          cmp_pword);
}

/**
 * Procedura zakończenia przetwarzania obecnej grupy i dodanie jej do tablicy
 * wszystkich groups oraz następne zresetowanie jej, aby pakować tam kolejne */
static void conclude_group(Group* group, Groups* all_groups)
{
  /* długość tablicy indeksów musi być zwiększona o 1 ponieważ na ich końcu
   * zapisuję 0 jako swoisty znacznik końca grupy */
  size_t group_size = (group->used + 1) * sizeof(size_t);
  size_t* new_group = malloc(group_size);

  if (!new_group)
    exit(1);

  /* zapisuję tam tabliczkę indeksów */
  memcpy(new_group, group->val, sizeof(size_t) * group->used);
  /* wspomniane 0 jako sygnalizator końca tablicy nrów wierszy */
  new_group[group->used] = 0;
  array_append(all_groups, sizeof(size_t*), &new_group);
  group->used = 0;
}

/**
 * Właściwe wypisanie wszystkich grup we wskazanym formacie.  */
static void print_all_groups(Groups all_groups)
{
  size_t i, j;

  for (i = 0; i < all_groups.used; ++i) {
    /* wypisanie indeksów wszystkich podobnych wierszy jednej grupy, prócz
     * ostatniego (0 -> koniec tablicy), ponieważ po nim nie będzie spacji,
     * a \n, więc printujemy go osobno */
    for (j = 0; all_groups.val[i][j + 1] != 0; ++j)
      printf("%lu ", all_groups.val[i][j]);

    printf("%lu\n", all_groups.val[i][j]);
  }
}

/**
 * Ułożenie outputu w odpowiedniej kolejności, wypisanie i zwolnienie pamięci */
static void conclude_grouping(Groups all_groups)
{
  /* sortuje po najmniejszych (pierwszych grâce à stabilność) indeksach linii */
  qsort(all_groups.val, all_groups.used, sizeof(size_t*), cmp_size_t_p);
  print_all_groups(all_groups);

  for (size_t i = 0; i < all_groups.used; ++i)
    free(all_groups.val[i]);

  free(all_groups.val);
}

/**
 * Znajdywanie indeksów podobnych sparsowanych (zał.: unormalizowanych) linijek
 * (w posortowanej tablicy) i następne zwrócenie ich w kolejnej tablicy */
static Groups find_similars(ParsedLine* plines, size_t len)
{
  Groups all_groups;
  bool new_group = true;
  size_t i = 0;
  /* obecnie rozważana linijka i wzorzec obecnie rozwijanej grupy */
  ParsedLine curr_line, group_line;
  /* przechowuję obecnie rozważaną grupę, którą recyklinguję na kolejne
   * (analogiczny mechanizm jak jeden bufor dla getline'a w input.c) */
  Group group;

  /* zakładam (może mylnie), że częściej zdarza się wiele grup rozczłonkowanych
   * niżli jedna wielka */
  array_init(&group, sizeof(size_t), SMALL_ARRAY_LENGTH);
  array_init(&all_groups, sizeof(size_t**), BIG_ARRAY_LENGTH);

  while (i < len) {
    curr_line = plines[i];

    if (new_group) {
      /* brak wzorca do porównania - nowa grupa */
      group_line = curr_line;
      array_append(&group, sizeof(size_t), &curr_line.line_num);
      new_group = !new_group;
      ++i;
    } else if (cmp_pline(&curr_line, &group_line)) {
      /* linijka różna od wzorca, kończymy obecną grupę */
      conclude_group(&group, &all_groups);
      new_group = true;
    } else {
      /* tablice słów są równe wtw linijki są podobne */
      array_append(&group, sizeof(size_t), &curr_line.line_num);
      ++i;
    }
  }

  /* zakończenie ostatniej grupy */
  if (group.used != 0)
    conclude_group(&group, &all_groups);

  free(group.val);

  return all_groups;
}

void write_groups(ParsedText t)
{
  Groups all_groups;

  if (t.used == 0)
    return;

  normalise(t.val, t.used);
  qsort(t.val, t.used, sizeof(ParsedLine), cmp_pline_stable);
  all_groups = find_similars(t.val, t.used);
  conclude_grouping(all_groups);
}
