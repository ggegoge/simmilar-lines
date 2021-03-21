#include "array.h"
#include "input.h"
#include "parse.h"
#include "group.h"


int main(void)
{
  ParsedText text;
  array_init(&text, sizeof(ParsedLine), 0);
  read_text(&text);
  write_groups(text);
  free_text(text);

  return 0;
}
