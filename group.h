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
void end_group(Group* group, Groups* all_groups);
void add_group(Group group, Groups* all_groups);

void print_all_groups(Groups all_groups);
void free_groups(Groups all_groups);

/* a whole lot of comparators one may need */
int pline_cmp(const void* l1, const void* l2);
/* porównania tablic */
int arrays_cmp(const void* a1, size_t len1, const void* a2, size_t len2,
               size_t width, int(*compare)(const void*, const void*));

/* porównanie ich elementów */
int cmp_whole(const void*, const void*);
int cmp_real(const void*, const void*);
int cmp_nan(const void*, const void*);
/* sortowanie wewnątrz wynikowych wierszy i na zewnątrz */
int cmp_size_t(const void* a, const void* b);
int cmp_size_t_p(const void* a, const void* b);

void normalise(PLine* plines, size_t len);

void find_similars(PLine* plines, size_t len);

#endif /* GROUP_H */
