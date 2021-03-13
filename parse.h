#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>

/* moduł do parse'owania */


typedef struct dyn_ints {
  size_t used, len;
  long long* val;
} DInts;

void append_ints(DInts*, long long new_num);

typedef struct dyn_floats {
  size_t used, len;
  double* val;
} DFloats;

void append_floats(DFloats**, double new_num);

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
  char* nans;
} PLine;

typedef struct parsed_text {
  size_t used, len;
  PLine* val;
} PText;


int add_parsed_int(PLine*, long long);
int add_parsed_float(PLine*, double);
int add_parsed_string(PLine*, char*);

int add_parsed_line(PText*, PLine);

void free_text(PText);

void error(size_t);


#include "input.h"
PLine parse(DLine);


#endif /* PARSE_H */
