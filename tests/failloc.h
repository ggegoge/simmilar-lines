
#include <stdlib.h>
#include <time.h>

int sranded = 0;

#define P 50

#define rnum() (!sranded ? (sranded=1, srand(time(NULL)), rand()%P) : rand()%P)
#define malloc(len) rnum() ? malloc(len) : NULL
#define realloc(ptr, len) rnum() ? realloc(ptr, len) : NULL
#define calloc(count, len) rnum() ? calloc(count, len) : NULL
