#ifndef GROUP_H
#define GROUP_H

#include "parse.h"

typedef struct group {
  size_t used, len;
  size_t* val;
} Group;

void write_groups(PText);
void end_group(Group*);


/* a whole lot of comparators one may need */
int pline_cmp(const void*, const void*);
/* compare arrays */
int arrays_cmp(const void* f1, size_t len1, const void* f2, size_t len2,
               size_t size, int(*cmpr)(const void*, const void*));

/* compare theirs elements */
int cmp_whole(const void*, const void*);
int cmp_real(const void*, const void*);
int cmp_nan(const void*, const void*);
int cmp_size_t(const void*, const void*);

void normalise(PLine*, size_t);

void add_similar(Group*, size_t);

void find_similars(PLine*, size_t);

#endif /* GROUP_H */
