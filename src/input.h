#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <stdbool.h>

#include "parse.h"

/**
 * Wczytanie ze standardowego wejścia i zapisanie sparsowanych wierszy
 * w tablicy ptext. */
void read_text(ParsedText* ptext);

#endif /* INPUT_H */
