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
static bool check_line(char* s, size_t line_num, size_t line_len);
static void parse(PLine* pline, const char* word);


PLine parseln(char* line, size_t line_num, size_t line_len)
{
  PLine pline;
  char* word = NULL;
  char* delims = " \t\n\v\f\r";

  /* sprawdzian zakresu i pustości */
  if (!check_line(line, line_num, line_len) || !(word = strtok(line, delims))) {
    pline.well_formed = false;
    return pline;
  }

  /* alokacja dopiero w momencie pewności, że dodamy tę linijkę */
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
  init(&pline.pwords, sizeof(PWord), 0);
  return pline;
}


static void free_line(PLine line)
{
  for (size_t i = 0; i < line.pwords.used; ++i)
    if (line.pwords.val[i].class == NEITHER)
      free(line.pwords.val[i].nan);

  free(line.pwords.val);
}

void free_text(PText text)
{
  for (size_t i = 0; i < text.used; ++i)
    free_line(text.val[i]);

  free(text.val);
}

static bool parse_whole(PLine* pline, const char* s)
{
  PWord pword;
  Whole num;
  char* err;
  /* musi być znak na początku, a jednocześnie nie może być ++... lub +-... itd */
  bool is_sign = (s[0] == '+' || s[0] == '-') && s[1] != '+' && s[1] != '-';

  /* goły znak to nei liczba */
  if (is_sign && strlen(s) == 1)
    return false;

  /* przyporządkowanie znaku */
  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  errno = 0;

  /* przed liczbami 8kowymi i 16kowymi nie pojawi się znak? psuje to estetykę
   * programu, prościej byłoby robić samo stroull( , , 0) */
  if (is_sign)
    num.abs = strtoull(s + 1, &err, 10);
  else
    num.abs = strtoull(s, &err, 0);

  /* "0x" to rzekomo legit liczba */
  if (strcmp(s, "0x") != 0 && (*err != '\0' || errno == ERANGE))
    return false;
  else {
    /* -0, 0 i +0 zapisuję jako +0 */
    if (num.abs == 0)
      num.sign = PLUS;

    pword.class = WHOLE;
    pword.whole = num;

    if (pline->pwords.len == 0)
      init(&pline->pwords, sizeof(PWord), SMALL_ARRAY);

    append(&pline->pwords, sizeof(PWord), &pword);
    return true;
  }
}

static bool parse_real(PLine* pline, const char* s)
{
  PWord pword;
  double num;
  Whole whole_num;
  char* err;

  /* nie chcę tu łapać za dużych intów z notacji intowej, sprawdzam czy wywołany
   * wcześniej parse_whole nie ustawił ostrzeżenia. Also: strtod nie ma opcji
   * specyfikacji systemu liczb, zatem odrzucone hexy typu +0x... -0x...
   * muszę ręcznie odrzucać po małpiemu. */
  if (errno == ERANGE || s[1] == 'x' || s[2] == 'x')
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

      pword.class = WHOLE;
      pword.whole = whole_num;

      if (pline->pwords.len == 0)
        init(&pline->pwords, sizeof(PWord), SMALL_ARRAY);

      append(&pline->pwords, sizeof(PWord), &pword);
      return true;
    }
  }

  pword.class = REAL;
  pword.real = num;

  if (pline->pwords.len == 0)
    init(&pline->pwords, sizeof(PWord), SMALL_ARRAY);

  append(&pline->pwords, sizeof(PWord), &pword);

  return true;
}

/**
 * Alokacja pamięci pod string @s i następnie dodanie go do plinijki @pline */
static void new_parsed_nan(PLine* pline, const char* s)
{
  PWord pword;
  size_t word_len = strlen(s);
  char* new_nan = (char*) malloc(word_len + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  pword.class = NEITHER;
  pword.nan = new_nan;

  if (pline->pwords.len == 0)
    init(&pline->pwords, sizeof(PWord), SMALL_ARRAY);

  append(&pline->pwords, sizeof(PWord), &pword);

  for (size_t i = 0; i < word_len; ++i)
    pline->pwords.val[pline->pwords.used - 1].nan[i] = tolower(s[i]);
}
/**
 * Próba sparsowania @word na 3 możliwe sposoby. */
static void parse(PLine* pline, const char* word)
{
  if (!(parse_whole(pline, word) || parse_real(pline, word)))
    new_parsed_nan(pline, word);
}

/* sprawdzian zakresu znakow */
static bool check_line(char* s, size_t line_num, size_t line_len)
{
  for (size_t i = 0; i < line_len; ++i) {
    if (!isspace(s[i]) && (s[i] < MIN_WORD_ASCII || s[i] > MAX_WORD_ASCII)) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    }
  }

  return true;
}
