#include <stdio.h>
#include <math.h>

#include "input.h"
#include "parse.h"

int main(void)
{
  PText text;


  printf("Wpisz rzeczy:\n");
  read_text(&text);
  free_text(text);
  
  return 0;
}
