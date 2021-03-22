#include <stdlib.h>
#include <time.h>

int sranded = 0;

#define P 30

#define rnum() (!sranded ? (sranded=1, srand(time(NULL)), rand()%P) : rand()%P)
#define malloc(len) rnum() ? malloc(len) : NULL
#define realloc(ptr, len) rnum() ? realloc(ptr, len) : NULL
