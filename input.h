#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdbool.h>

/* input.h */

/**
 *  reads text from stdin and packs it into a DText dynamic array
 */

/* read text from stdin without all this malloc bs */
void readln(char**, size_t*, bool*, bool*);

#endif /* INPUT_H */
