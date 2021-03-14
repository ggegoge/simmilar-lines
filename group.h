#ifndef GROUP_H
#define GROUP_H

#include <stdbool.h>

#include "parse.h"

typedef struct group {
  size_t used, len;
  size_t* idxs;
} Group;

bool write_groups(PText);


#endif /* GROUP_H */
