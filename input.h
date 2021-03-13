#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>

#define INIT_ARR_SIZE 2137
#define ARR_EXPAND 1.25



/* input.h */
/**
 * @used -- how many elements inserted by the user,
 * @len -- the actual space in memory taken by the arrays,
 * @line_num -- only with DLine, the line number,
 * @val -- the actual array. acces elts by `arr.val[i]`,
 */

typedef struct dyn_line {
  size_t used, len, line_num;
  int commentary, initialised;
  char* val;
} DLine;

/* typedef struct dyn_text {
 *   size_t used, len;
 *   DLine* val;
 * } DText; */


/* new struct initialisation */
DLine init_line(size_t, size_t);


#include "parse.h"
/**
 *  reads text from stdin and packs it into a DText dynamic array
 */
int read_text(PText*);

/**
 *  Frees the memory used by @text and every line contained there
 */

#endif /* INPUT_H */
