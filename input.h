#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdbool.h>

#define INIT_ARR_SIZE 16
#define ARR_EXPAND 1.25



/* input.h */

/**
 * Calculate new suitable length for an array that is currently @curr_len
 * "fields" long. */
static inline size_t new_len(size_t curr_len)
{
  return ((size_t)curr_len + (curr_len >> 2) + 6) & ~(size_t)3;
}

/**
 *  reads text from stdin and packs it into a DText dynamic array
 */

/* read text from stdin without all this malloc bs */
void readln(char**, size_t*, bool*, bool*);

#endif /* INPUT_H */
