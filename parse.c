#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "parse.h"
#include "input.h"

PLine parseln(char* line, size_t line_num)
{
  PLine pline;
  char* word = NULL;
  char* delims = " \t\n\v\f\r";
  pline.line_num = line_num;
  printf("parsing line #%lu ", pline.line_num);

  for (size_t i = 0; i < strlen(line); ++i)
    printf("%c", line[i]);

  printf("splitting #%lu line into words\n", line_num);

  word = strtok(line, delims);

  while (word) {
    parse(&pline, word);
    printf("\"%s\"     ", word);
    word = strtok(NULL, delims);

    if (!(pline.well_formed = check_word(word, line_num)))
      return pline;
  }

  printf("\n-------------\n");

  return pline;
}

PText init_ptext()
{
  PText ptext;
  ptext.len = INIT_ARR_SIZE;
  ptext.used = 0;
  ptext.val = (PLine*) malloc(INIT_ARR_SIZE * sizeof(PLine));
  return ptext;
}

void free_text(PText text)
{
  if (text.val) {
    free(text.val);
    text.val = NULL;
  }
}

void parse(PLine* pline, const char* word)
{
  bool succ = parse_whole(pline, word) || parse_real(pline, word) ||
              parse_nan(pline, word);

  if (!succ)
    fprintf(stderr, "this shit aint parsable!\n");
}


bool parse_whole(PLine* pline, const char* s)
{
  Whole num;
  bool is_sign = s[0] == '+' || s[0] == '-';
  char* err;

  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  if (is_sign)
    num.abs = strtoull(s + 1, &err, 0);
  else
    num.abs = strtoull(s, &err, 0);

  if (*err == '\0' || errno == ERANGE)
    return false;
  else {
    add_parsed_whole(pline, num);
    return true;
  }

}


void add_parsed_whole(PLine* pline, Whole num)
{
  pline->wholes.used++;

  if (pline->wholes.used >= pline->wholes.len) {
    pline->wholes.len = new_len(pline->wholes.len);
    pline->wholes.val = (Whole*) realloc(pline->wholes.val,
                                           pline->wholes.len * sizeof(Whole));

    if (!pline->wholes.val)
      exit(1);
  }

  pline->wholes.val[pline->wholes.used - 1] = num;
}

bool parse_real(PLine* pline, const char* s)
{
  double num;
  char* err;

  num = strtod(s, &err);
  
  if (*err == '\0' || errno == ERANGE)
    return false;
  else {
    add_parsed_real(pline, num);
    return true;
  }

}

void add_parsed_real(PLine* pline, double num)
{
  pline->wholes.used++;

  if (pline->reals.used >= pline->reals.len) {
    pline->reals.len = new_len(pline->reals.len);
    pline->reals.val = (double*) realloc(pline->reals.val,
                                           pline->reals.len * sizeof(double));

    if (!pline->reals.val)
      exit(1);
  }

  pline->reals.val[pline->reals.used - 1] = num;
}


void add_parsed_line(PText* ptext, PLine pline)
{
  ++ptext->used;

  if (ptext->used >= ptext->len) {
    ptext->len = new_len(ptext->len);
    ptext->val = (PLine*) realloc(ptext->val, ptext->len * sizeof(PLine));

    if (!ptext->val)
      exit(1);
  }

  ptext->val[ptext->used - 1] = pline;
}

void add_parsed_float(PLine_old* pline, double num)
{
  pline->floats.used++;

  if (pline->floats.used >= pline->floats.len) {
    pline->floats.len = new_len(pline->floats.len);
    pline->floats.val = (double*) realloc(pline->floats.val,
                                          pline->floats.len * sizeof(double));

    if (!pline->floats.val)
      exit(1);
  }

  pline->floats.val[pline->floats.used - 1] = num;
}

void add_parsed_int(PLine_old* pline, long long num)
{
  pline->ints.used++;

  if (pline->ints.used >= pline->ints.len) {
    pline->ints.len = new_len(pline->ints.len);
    pline->ints.val = (long long*) realloc(pline->ints.val,
                                           pline->ints.len * sizeof(long long));

    if (!pline->ints.val)
      exit(1);
  }

  pline->ints.val[pline->ints.used - 1] = num;
}

void add_parsed_string(PLine* pline, char* str)
{
  pline->nans.used++;

  if (pline->nans.used >= pline->nans.len) {
    pline->nans.len = new_len(pline->nans.len);
    pline->nans.val = (char**) realloc(pline->nans.val, new_len(pline->nans.len));

    if (!pline->nans.val)
      exit(1);
  }

  pline->nans.val[pline->nans.used - 1] = str;
}

bool check_word(char* w, size_t line_num)
{
  for (size_t i = 0; i < strlen(w); ++i) {
    if (w[i] < MIN_WORD_ASCII || w[i] > MAX_WORD_ASCII) {
      fprintf(stderr, "ERROR %lu", line_num);
      return false;
    }
  }

  return true;
}

