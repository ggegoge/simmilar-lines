#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

PLine parseln(char* line, size_t line_num)
{
  PLine pline = init_pline(line_num);
  char* word = NULL;
  char* delims = " \t\n\v\f\r";

  word = strtok(line, delims);

  /* pusta linia? */
  if (!word) {
    pline.well_formed = false;
    return pline;
  }

  while (word) {
    /* sprawdzian, czy znaki spelniaja zalozenia */
    if (!(pline.well_formed = check_word(word, line_num)))
      return pline;

    parse(&pline, word);
    word = strtok(NULL, delims);
  }

  return pline;
}

PText init_ptext()
{
  PText ptext;
  ptext.len = BIG_ARRAY;
  ptext.used = 0;
  ptext.val = (PLine*) malloc(ptext.len * sizeof(PLine));
  return ptext;
}

PLine init_pline(size_t line_num)
{
  PLine pline;
  pline.line_num = line_num;
  pline.well_formed = true;

  pline.wholes.len = SMALL_ARRAY;
  pline.wholes.used = 0;
  pline.wholes.val = (Whole*) malloc(pline.wholes.len * sizeof(Whole));

  pline.reals.len = SMALL_ARRAY;
  pline.reals.used = 0;
  pline.reals.val = (double*) malloc(pline.reals.len * sizeof(double));

  pline.nans.len = SMALL_ARRAY;
  pline.nans.used = 0;
  pline.nans.val = (char**) malloc(pline.nans.len * sizeof(char*));

  return pline;
}


void free_text(PText text)
{
  for (size_t i = 0; i < text.used; ++i)
    free_line(text.val[i]);

  if (text.val) {
    free(text.val);
    text.val = NULL;
  }

}

void free_line(PLine line)
{
  free(line.reals.val);
  free(line.wholes.val);

  for (size_t i = 0; i < line.nans.used; ++i)
    free(line.nans.val[i]);

  free(line.nans.val);

}

void parse(PLine* pline, const char* word)
{
  if (parse_whole(pline, word) || parse_real(pline, word))
    return;

  new_parsed_nan(pline, word);
}


bool parse_whole(PLine* pline, const char* s)
{
  Whole num;
  bool is_sign = (s[0] == '+' || s[0] == '-');
  char* err;

  if (is_sign && strlen(s) == 1)
    return false;

  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  errno = 0;

  if (is_sign)
    num.abs = strtoull(s + 1, &err, 0);
  else
    num.abs = strtoull(s, &err, 0);

  if (*err != '\0' || errno == ERANGE)
    return false;
  else {
    append(&pline->wholes, sizeof(Whole), &num);
    return true;
  }
}

bool parse_real(PLine* pline, const char* s)
{
  double num;
  Whole whole_num;
  char* err;

  /* it is not a real number but a too large of an integer */
  if (errno == ERANGE)
    return false;

  errno = 0;
  num = strtod(s, &err);

  /* nan is a nan as the name specifies */
  if (isnan(num))
    return false;

  /* check if it is not a misbehaving integer. idk how toooo */
  if (*err == '\0' && isfinite(num) &&
      fabs(num) == (unsigned long long)fabs(num)) {
    if (fabs(num) <= ULLONG_MAX) {
      whole_num.abs = (unsigned long long) fabs(num);

      if (num > 0)
        whole_num.sign = PLUS;
      else
        whole_num.sign = MINUS;

      append(&pline->wholes, sizeof(Whole), &whole_num);
      return true;
    }
  }

  if (*err != '\0' || errno == ERANGE)
    return false;
  else {
    append(&pline->reals, sizeof(double), &num);
    return true;
  }

}


void new_parsed_nan(PLine* pline, const char* s)
{
  char* new_nan = (char*) malloc(strlen(s) + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  append(&pline->nans, sizeof(char*), &new_nan);

  for (size_t i = 0; i < strlen(s); ++i)
    pline->nans.val[pline->nans.used - 1][i] = tolower(s[i]);
}

bool check_word(char* w, size_t line_num)
{
  for (size_t i = 0; i < strlen(w); ++i) {
    if (w[i] < MIN_WORD_ASCII || w[i] > MAX_WORD_ASCII) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    }
  }

  return true;
}
