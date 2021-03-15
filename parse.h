#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>

#include "input.h"

/* the range of acceptable ascii chars */
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126

/* moduł do parse'owania */


typedef struct whole {
  unsigned long long abs;
  enum { PLUS, MINUS } sign;
} Whole;


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

/* old structs... */
typedef struct dyn_ints {
  size_t used, len;
  long long* val;
} DInts;

/* void append_ints(DInts*, long long new_num); */

typedef struct dyn_floats {
  size_t used, len;
  double* val;
} DFloats;


typedef struct dyn_strs {
  size_t used, len;
  char** val;
} DStrs;

/* void append_floats(DFloats**, double new_num); */

/* i'll store nonnumericals as a string? idk...
 * thats not the best idea tbf. I have to be able to distinguish those single
 * words. Perhaps I need a special container that can serve as it should?
 * First read as str and get the lenght? not feasible I'm afraid. ugh */

typedef struct parsed_line {
  size_t line_num;
  bool well_formed;
  char* sig;
  struct dyn_wholes wholes;
  struct dyn_reals reals;
  struct dyn_nans nans;
} PLine;


typedef struct parsed_line_old {
  size_t line_num;
  bool well_formed;
  char* sig;
  DInts ints;
  DFloats floats;
  DStrs nans;
} PLine_old;

typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;


/* initialisation and freeing of the PText */
PText init_ptext();
PLine init_pline(size_t);
void free_text(PText);

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
bool parse_nan(PLine*, const char*);

/* adding any of the types to the array in PLine */
/* void add_parsed_int(PLine*, long long);
 * void add_parsed_float(PLine*, double);
 * void add_parsed_string(PLine*, char*); */

/* adding any of the types to the array in PLine */
void add_parsed_whole(PLine*, Whole);
void add_parsed_real(PLine*, double);
void add_parsed_nan(PLine*, const char*);


/* add one PLine to PText */
void add_parsed_line(PText*, PLine);

#endif /* PARSE_H */
