#ifndef GROUP_H
#define GROUP_H

#include "parse.h"

/**
 * Pojedyncza grupa podobnych wierszy, tablica dynamiczna rodem z array.h,
 * w swoich trzewiach trzyma indeksy wszystkich podobnych wierszy danej grupy. */
typedef struct Group {
  size_t used, len;
  size_t* val;
} Group;

/**
 * Kolejna tablica dynamiczna w wiadomym stylu. Trzyma wiele grup, każdą jako
 * tablicę indeksów. */
typedef struct Groups {
  size_t used, len;
  size_t** val;
} Groups;

/**
 * Funkcja służy wypisaniu wszystkich podobnych wierszy z @ptext na stdout. */
void write_groups(ParsedText ptext);

#endif /* GROUP_H */
