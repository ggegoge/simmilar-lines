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

void write_groups(PText);
void end_group(Group*, Groups*);
void add_group(Group, Groups*);

void print_all_groups(Groups);
void free_groups(Groups);

/* a whole lot of comparators one may need */
int pline_cmp(const void*, const void*);
/* porównania tablic */
int arrays_cmp(const void* f1, size_t len1, const void* f2, size_t len2,
               size_t size, int(*)(const void*, const void*));

/* porównanie ich elementów */
int cmp_whole(const void*, const void*);
int cmp_real(const void*, const void*);
int cmp_nan(const void*, const void*);
/* sortowanie wewnątrz wynikowych wierszy i na zewnątrz */
int cmp_size_t(const void*, const void*);
int cmp_size_t_p(const void*, const void*);

void normalise(PLine*, size_t);

void add_similar(Group*, size_t);

void find_similars(PLine*, size_t);

#endif /* GROUP_H */
