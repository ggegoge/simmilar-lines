#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "array.h"
#include "input.h"
#include "parse.h"
#include "group.h"


int main(void)
{
  PText text;
  init(&text, sizeof(PLine), 0);
  read_text(&text);
  write_groups(text);
  free_text(text);

  return 0;
}
