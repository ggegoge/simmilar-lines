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
  char* val;
} DLine;

typedef struct dyn_text {
  size_t used, len;
  DLine* val;
} DText;


/* new struct initialisation */
DLine new_line(size_t, size_t);
DText new_text(size_t);

/**
 *  reads text from stdin and packs it into a DText dynamic array
 */
DText read_text();

/**
 *  Frees the memory used by @text and every line contained there 
 */
void free_text(DText text);

#endif /* INPUT_H */
