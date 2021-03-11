#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "input.h"

/**
 * A module for reading next lines from the stdin and then neatly packing them
 * into dynamic arrays DText and DLine
 */


/* possible improvements (?):
 *   - save normal strings to output text (DText) not DLine-s?
 *     that would be easily accomplished with eg:
 *     append_to_line("\0"); append_to_text(line.val)
 *     and would help with further tokenisation, splitting etc
 *   - EOF detection? */


/**
 *  Frees the memory contained by @text.val and every line contained there */
void free_text(DText text)
{
  DLine line;
  size_t i;

  for (i = 0; i < text.len; i++) {
    line = text.val[i];
    free(line.val);
  }

  free(text.val);
}

size_t new_len(size_t curr_len)
{
  return (size_t) ceil(((double)curr_len * ARR_EXPAND));
}

void append_to_line(DLine* line, char new_char)
{
  line->used++;

  if (line->used >= line->len) {
    line->len = new_len(line->len);
    line->val = (char*)realloc(line->val, line->len * sizeof(char));

    if (!line->val)
      fprintf(stderr, "REALLOC FAILURE\n");
  }

  line->val[line->used - 1] = new_char;
}

void append_to_text(DText* text, DLine new_line)
{
  text->used++;

  if (text->used >= text->len) {
    text->len = new_len(text->len);
    text->val = (DLine*)realloc(text->val, text->len * sizeof(DLine));

    if (!text->val)
      fprintf(stderr, "REALLOC FAILURE\n");

  }

  text->val[text->used - 1] = new_line;
}

DLine new_line(size_t init_len, size_t line_num)
{
  DLine line;
  line.line_num = line_num;
  line.used = 0;
  line.len = init_len;
  line.val = (char*)malloc(init_len * sizeof(char));

  if (!line.val)
    fprintf(stderr, "MALLOC FAILURE\n");

  return line;
}

DText new_text(size_t init_len)
{
  DText text;
  text.used = 0;
  text.len = init_len;
  text.val = (DLine*)malloc(init_len * sizeof(DLine));

  if (!text.val)
    fprintf(stderr, "MALLOC FAILURE\n");

  return text;
}

DLine read_line(size_t line_num)
{
  char c = '0';
  DLine line = new_line(INIT_ARR_SIZE, line_num);
  line.line_num = line_num;

  while (c != '\n' && scanf("%c", &c) == 1) {      /* c != EOF */
    /* scanf("%c", &c); */
    append_to_line(&line, c);
  }

  return line;
}

DText read_text()
{
  size_t lin_num = 0;
  DText text = new_text(INIT_ARR_SIZE);
  DLine line;

  while (!feof(stdin)) {
    line = read_line(lin_num);
    append_to_text(&text, line);
    ++lin_num;
  }

  return text;
}
