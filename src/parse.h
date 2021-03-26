#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>

/**
 * Moduł do parse'owania linijek tekstu. */

/**
 * Typ służący reprezentacji liczb całkowitych. Składuję osobno wart. absolutną
 * liczby @abs i jej znak @sing, tak by mieściło się w jednym typie wszystko */
typedef struct Whole {
  unsigned long long abs;
  enum { PLUS, MINUS } sign;
} Whole;

/* Liczby rzeczywiste nazywam real i trzymam je jako double'e. Nieliczby
 * nazywam nanami i trzymam je jako stringi */

/**
 * Pojedyncze sparsowane słowo. @class mówi o typie słowa, anonimowa unia to
 * jego zawartość (odpowiednie przegródki @whole, @real, @nan).
 * Okazuje się, że słowo NAN jest zarezerwowane stąd NEITHER */
typedef struct ParsedWord {
  enum { WHOLE, REAL, NEITHER } class;
  union {
    Whole whole;
    double real;
    char* nan;
  };
} ParsedWord;

/**
 * Tablica na sparsowane słowa jako osobna struktura celem kompatybilności ze
 * zdefiniowanymi w array.h funkcjami. */
struct dyn_pwords {
  size_t used, len;
  ParsedWord* val;
};

/**
 * Pojedynczy przerobiony wiersz.
 * Trzyma nr linii @line_num, w @well_formed informację czy ta jest akceptowalna
 * do przetworzenia tj czy brak tam nielegalnnych znaków lub czy nie jest pusta.
 * @pwords przechowuje wszystkie sparsowane słowa z danej linijki. */
typedef struct ParsedLine {
  size_t line_num;
  bool well_formed;
  struct dyn_pwords pwords;
} ParsedLine;

/**
 * ParsedText -- dynamiczna tablica à la moduł array przechowuje ParsedLine'y */
typedef struct ParsedText {
  size_t used, len;
  ParsedLine* val;
} ParsedText;

/**
 * Zwalnianie pamięci zajmowanej przez taką tablicę i jej składowe. */
void free_text(ParsedText ptext);

/**
 * Przetworzenie pojedynczej linii @line czyli @line_num-tego wiersza. */
ParsedLine parse_line(char* line, size_t line_num, size_t line_len);

#endif /* PARSE_H */
