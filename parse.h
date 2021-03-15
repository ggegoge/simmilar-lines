#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>


/* the range of acceptable ascii chars */
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126

/* moduł do parse'owania */

/* i store numbers and there signs so that they fit into one type */
typedef struct whole {
  unsigned long long abs;
  enum { PLUS, MINUS } sign;
} Whole;


/* dynamic arrays for all the possible types of words */

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


/* a single parsed line (PLine) */
typedef struct parsed_line {
  size_t line_num;
  bool well_formed;
  char* sig;
  struct dyn_wholes wholes;
  struct dyn_reals reals;
  struct dyn_nans nans;
} PLine;

/* dynamic array of parsed lines (PText) */
typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;

/* initialisation and freeing of the PText and of a pline */
PText init_ptext();
PLine init_pline(size_t);
void free_text(PText);
void free_line(PLine);

/* parse single line */
PLine parseln(char*, size_t);

/* check wheter all chars in a word stay in the ascii range of [33, 126] */
bool check_word(char*, size_t);

/* parsing */
void parse(PLine*, const char*);

/* a parser for each type -- long long, unsinged long long, double, string */
/* lovely names: */
bool parsell(PLine*, const char*);
bool parseull(PLine*, const char*);
bool parsed(PLine*, const char*);
bool parsestr(PLine, const char*);

/* hideous names: */
bool parse_whole(PLine*, const char*);
bool parse_real(PLine*, const char*);

/* adding any of the types to the array in PLine */
void add_parsed_whole(PLine*, Whole);
void add_parsed_real(PLine*, double);
void add_parsed_nan(PLine*, const char*);

/* add one PLine to PText */
void add_parsed_line(PText*, PLine);

#endif /* PARSE_H */
