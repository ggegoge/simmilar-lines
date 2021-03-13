#ifndef POLY_PARSE_H
#define POLY_PARSE_H

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
  char* sig;
  DInts ints;
  DFloats floats;
  char* nans;
} PLine;

/* polymorphic version: */

#define CHUNK_SIZE sizeof(long long)

void add_parsed_int(PLine*, void*);
void add_parsed_float(PLine*, void*);
void add_parsed_string(PLine*, void*);


#include "input.h"
PLine parse(DLine);

#endif /* POLY_PARSE_H */
