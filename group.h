#ifndef GROUP_H
#define GROUP_H

#include "parse.h"

typedef struct dyn_group {
  size_t used, len;
  size_t* val;
} Group;

typedef struct dyn_all_groups {
  size_t used, len;
  size_t** val;
} Groups;

void write_groups(PText ptext);

#endif /* GROUP_H */
