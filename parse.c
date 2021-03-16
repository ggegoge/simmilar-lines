#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

static PLine init_pline(size_t);
static bool check_word(char*, size_t);
static void parse(PLine* pline, const char* word);


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

static PLine init_pline(size_t line_num)
{
  PLine pline;
  pline.line_num = line_num;
  pline.well_formed = true;

  init(&pline.wholes, sizeof(Whole), SMALL_ARRAY);
  init(&pline.reals, sizeof(double), SMALL_ARRAY);
  init(&pline.nans, sizeof(char*), SMALL_ARRAY);

  return pline;
}


void free_text(PText text)
{
  for (size_t i = 0; i < text.used; ++i)
    free_line(text.val[i]);

  free(text.val);
}

void free_line(PLine line)
{
  free(line.reals.val);
  free(line.wholes.val);

  for (size_t i = 0; i < line.nans.used; ++i)
    free(line.nans.val[i]);

  free(line.nans.val);
}

static bool parse_whole(PLine* pline, const char* s)
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

  /* przed liczbami 8kowymi i 16kowymi nie pojawi się znak? */
  if (is_sign)
    num.abs = strtoull(s + 1, &err, 10);
  else
    num.abs = strtoull(s, &err, 0);

  if (*err != '\0' || errno == ERANGE)
    return false;
  else {
    append(&pline->wholes, sizeof(Whole), &num);
    return true;
  }
}

static bool parse_real(PLine* pline, const char* s)
{
  double num;
  Whole whole_num;
  char* err;

  /* nie chcę tu łapać za dużych intów z notacji intowej, sprawdzam czy wywołany
   * wcześniej parse_whole nie ustawił ostrzeżenia. */
  if (errno == ERANGE)
    return false;

  errno = 0;
  num = strtod(s, &err);

  /* odchodzimy odczyt się nie udał */
  if (*err != '\0' || errno == ERANGE || isnan(num))
    return false;

  /* sprawdźmy, czy to nie jest int w przebraniu floata */
  if (isfinite(num) && fabs(num) == (unsigned long long)fabs(num)) {
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

  /* ostatnia możliwość: to istotnie liczba rzeczywista */
  append(&pline->reals, sizeof(double), &num);
  return true;
}

static void new_parsed_nan(PLine* pline, const char* s)
{
  char* new_nan = (char*) malloc(strlen(s) + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  append(&pline->nans, sizeof(char*), &new_nan);

  for (size_t i = 0; i < strlen(s); ++i)
    pline->nans.val[pline->nans.used - 1][i] = tolower(s[i]);
}

static void parse(PLine* pline, const char* word)
{
  if (parse_whole(pline, word) || parse_real(pline, word))
    return;

  new_parsed_nan(pline, word);
}


static bool check_word(char* w, size_t line_num)
{
  for (size_t i = 0; i < strlen(w); ++i) {
    if (w[i] < MIN_WORD_ASCII || w[i] > MAX_WORD_ASCII) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    }
  }

  return true;
}
