#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>


/* Zakres akceptowalnych znaków ascii. */
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126

/**
 * Moduł do parse'owania linijek tekstu. */

/* składuję osobno wart. abs. liczby i jej znak, tak by mieściło się w jednym
 * typie wszystko */
typedef struct whole {
  unsigned long long abs;
  enum { PLUS, MINUS } sign;
} Whole;

/* Liczby rzeczywiste nazywam real i trzymam je jako double'e. Nie liczby
 * nazywam nanami i trzymam je jako stringi */

/**
 * Pojedyncze sparsowane słowo. @class mówi o typie słowa, anonimowa unia to
 * jego zawartość (odpowiednie przegródki @whole, @real, @nan).
 * Okazuje się, że NAN jest zarezerwowane... */
typedef struct parsed_word {
  enum { WHOLE, REAL, NEITHER } class;
  union {
    Whole whole;
    double real;
    char* nan;
  };
} PWord;

struct dyn_pwords {
  size_t used, len;
  PWord* val;
};

/**
 * Pojedynczy przerobiony wiersz (ang. parsed line --> PLine).
 * Trzyma nr linii @line_num, w @well_formed informację czy ta jest akceptowalna
 * do przetworzenia tj czy brak tam nielegalnnych znaków lub czy nie jest pusta,
 * @wholes, @reals i @nans to trzy podręczne tablice dynamiczne na wiadome typy. */
typedef struct parsed_line {
  size_t line_num;
  bool well_formed;
  struct dyn_pwords pwords;
} PLine;

/*  PText tj parsed text -- przechowuje PLine'y */
typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;

/* zwalnianie pamięci */
void free_text(PText ptext);

/**
 * przetworzenie pojedynczej linii @line czyli @line_num-tego wiersza. */
PLine parseln(char* line, size_t line_num, size_t line_len);

/* najpiękniejsze funkcje świata */
bool parsell(PLine*, const char*);
bool parseull(PLine*, const char*);
bool parsed(PLine*, const char*);
bool parsestr(PLine, const char*);

#endif /* PARSE_H */
