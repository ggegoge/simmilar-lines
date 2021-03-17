#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

static PLine init_pline(size_t);
static void free_line(PLine);
static bool check_line(char* s, size_t line_num);
static void parse(PLine* pline, const char* word);


PLine parseln(char* line, size_t line_num)
{
  PLine pline;
  char* word = NULL;
  char* delims = " \t\n\v\f\r";

  /* sprawdzian zakresu i pustości */
  if (!check_line(line, line_num) || !(word = strtok(line, delims))) {
    pline.well_formed = false;
    return pline;
  }

  pline = init_pline(line_num);

  while (word) {
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
  init(&pline.wholes, sizeof(Whole), 0);
  init(&pline.reals, sizeof(double), 0);
  init(&pline.nans, sizeof(char*), 0);

  return pline;
}

void init_ptext(PText* ptext)
{
  ptext->len = 0;
  ptext->used = 0;
  ptext->val = NULL;
}

static void free_line(PLine line)
{
  free(line.reals.val);
  free(line.wholes.val);

  for (size_t i = 0; i < line.nans.used; ++i)
    free(line.nans.val[i]);

  free(line.nans.val);
}

void free_text(PText text)
{
  for (size_t i = 0; i < text.used; ++i)
    free_line(text.val[i]);

  free(text.val);
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

  /* "0x" to rzekomo legit liczba */
  if (strcmp(s, "0x") != 0 && (*err != '\0' || errno == ERANGE))
    return false;
  else {
    if (num.abs == 0)
      num.sign = PLUS;

    if (pline->wholes.len == 0)
      init(&pline->wholes, sizeof(Whole), SMALL_ARRAY);

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
   * wcześniej parse_whole nie ustawił ostrzeżenia. Also: strtod nie ma opcji
   * specyfikacji systemu liczb, zatem odrzucone hexy typu +0x... -0x...
   * muszę ręcznie odrzucać po małpiemu. */
  if (errno == ERANGE || s[2] == 'x')
    return false;

  errno = 0;
  num = strtod(s, &err);

  /* czy poprawna rzeczywista */
  if (*err != '\0' || errno == ERANGE || isnan(num))
    return false;

  /* sprawdźmy, czy to nie jest int w przebraniu floata */
  if (isfinite(num) && fabs(num) == (unsigned long long) fabs(num)) {
    if (fabs(num) <= ULLONG_MAX) {
      whole_num.abs = (unsigned long long) fabs(num);

      if (num >= 0)
        whole_num.sign = PLUS;
      else
        whole_num.sign = MINUS;

      if (pline->wholes.len == 0)
        init(&pline->wholes, sizeof(Whole), SMALL_ARRAY);

      append(&pline->wholes, sizeof(Whole), &whole_num);
      return true;
    }
  }

  /* to istotnie liczba rzeczywista */
  if (pline->reals.len == 0)
    init(&pline->reals, sizeof(double), SMALL_ARRAY);

  append(&pline->reals, sizeof(double), &num);
  return true;
}

static void new_parsed_nan(PLine* pline, const char* s)
{
  char* new_nan = (char*) malloc(strlen(s) + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);

  if (pline->nans.len == 0)
    init(&pline->nans, sizeof(char*), SMALL_ARRAY);

  append(&pline->nans, sizeof(char*), &new_nan);

  for (size_t i = 0; i < strlen(s); ++i)
    pline->nans.val[pline->nans.used - 1][i] = tolower(s[i]);
}

static void parse(PLine* pline, const char* word)
{
  if (!(parse_whole(pline, word) || parse_real(pline, word)))
    new_parsed_nan(pline, word);
}

/* sprawdzian zakresu znakow */
static bool check_line(char* s, size_t line_num)
{
  for (size_t i = 0; i < strlen(s); ++i) {
    if (!isspace(s[i]) && (s[i] < MIN_WORD_ASCII || s[i] > MAX_WORD_ASCII)) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    }
  }

  return true;
}
