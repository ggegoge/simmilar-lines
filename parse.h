#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>

#include "input.h"

/* moduł do parse'owania */


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
  int alloc_stat, well_formed;
  char* sig;
  DInts ints;
  DFloats floats;
  DStrs nans;
} PLine;

typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;


/* parsing */
bool try_int(const char*, PLine*);
bool try_float(const char*, PLine*);
bool try_str(const char*, PLine*);


/* adding any of the types to the array in PLine */
bool add_parsed_int(PLine*, long long);
bool add_parsed_float(PLine*, double);
bool add_parsed_string(PLine*, char*);

/* add one PLine to PText */
bool add_parsed_line(PText*, PLine);

/* initialisation and freeing of the PText */
PText init_ptext();
void free_text(PText);

void error(size_t);


/* PLine parse(DLine); */
PLine parse_getv(char*, size_t);

#endif /* PARSE_H */
