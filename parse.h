#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>

#include "input.h"

/* moduł do parse'owania */


typedef struct dyn_ints {
  size_t used, len;
  long long* val;
} DInts;

void append_ints(DInts*, long long new_num);

typedef struct dyn_floats {
  size_t used, len;
  long long* val;
} DFloats;

void append_floats(DFloats**, double new_num);

typedef struct parsed_line {
  size_t line_num;  
  char* sig;
  DInts ints;
  DFloats floats;
  char** nans;
} PLine;

/* void add_parsed_int(PLine, long long);
 * void add_parsed_float(PLine, double);
 * void add_parsed_string(PLine, char*); */
void add_parsed_int(PLine, void*);
void add_parsed_float(PLine, void*);
void add_parsed_string(PLine, void*);


PLine parse(DLine);

void error(size_t);

#endif /* PARSE_H */
