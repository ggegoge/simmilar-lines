#include <stdio.h>
#include <math.h>

#include "input.h"

int main(void)
{
  DText text;
  size_t i, j;

  printf("Wpisz rzeczy:\n");
  text = read_text();
  for (i = 0; i < text.used; i++) {
    for (j=0; j < text.val[i].used; j++)
      printf("%c", text.val[i].val[j]);
    printf("\n");
  }
  free_text(text);
  
  return 0;
}
