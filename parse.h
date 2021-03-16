#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>


/* Zakres akceptowalnych znaków ascii. */
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126

/**
 * Moduł do parse'owania linijek textu. */

/* składuję osobno wart abs liczby i jej wartość da by mieściło się w jednym
 * typie. */
typedef struct whole {
  unsigned long long abs;
  enum { PLUS, MINUS } sign;
} Whole;

/* Liczby rzeczywiste nazywam real i trzymam je jako double'e. Nie liczby
 * nazywam nanami i trzymam je jako stringi */

/* dynamiczne tablice dla każdego typu i podtypu słów. bazowane na DynArr
 * z modułu array. objaśnienia pól ibidem */
struct dyn_wholes {
  size_t used, len;
  Whole* val;
};

struct dyn_reals {
  size_t used, len;
  double* val;
};

struct dyn_nans {
  size_t used, len;
  char** val;
};


/**
 * Pojedynczy przerobiony wiersz (ang. parsed line --> PLine).
 * Trzyma nr linii @line_num, w @well_formed informację czy ta jest akceptowalna
 * do przetworzenia tj czy brak tam nielegalnnych znaków lub czy nie jest pusta,
 * @wholes, @reals i @nans to trzy podręczne tablice dynamiczne na wiadome typy. */
typedef struct parsed_line {
  size_t line_num;
  bool well_formed;
  struct dyn_wholes wholes;
  struct dyn_reals reals;
  struct dyn_nans nans;
} PLine;

/*  PText tj parsed text -- przechowuje PLine'y */
typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;

/* inicjalizacja wartości oraz pamięci i jej zwalnianie */
PLine init_pline(size_t);
void free_text(PText);
void free_line(PLine);

/**
 * przetworzenie pojedynczej linii @line czyli @line_num-tego wiersza. */
PLine parseln(char* line, size_t line_num);

/* Sprawdzenie legalności słowa tj przynależności znaków do assi-kresu [33, 126] */
bool check_word(char*, size_t);

/**
 * przetwarzenie słowa @word i zapisanie go w @pline. */
void parse(PLine* pline, const char* word);

bool parsell(PLine*, const char*);
bool parseull(PLine*, const char*);
bool parsed(PLine*, const char*);
bool parsestr(PLine, const char*);

/* hideous names: */
bool parse_whole(PLine*, const char*);
bool parse_real(PLine*, const char*);

/* alokacja pamięci pod nieliczby */
void new_parsed_nan(PLine*, const char*);

#endif /* PARSE_H */
