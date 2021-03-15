#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "parse.h"
#include "input.h"

PLine parseln(char* line, size_t line_num)
{
  PLine pline = init_pline(line_num);
  char* word = NULL;
  char* delims = " \t\n\v\f\r";


  printf("parsing line #%lu ", pline.line_num);

  for (size_t i = 0; i < strlen(line); ++i)
    printf("%c", line[i]);

  printf("splitting #%lu line into words\n", line_num);

  word = strtok(line, delims);

  if (!word) {
    printf("pusta linjka\n");
    pline.well_formed = false;
    return pline;
  }

  while (word) {
    if (!(pline.well_formed = check_word(word, line_num)))
      return pline;

    parse(&pline, word);
    printf("\"%s\"     ", word);
    word = strtok(NULL, delims);
  }

  printf("summary of line no #%lu:\n", line_num);
  printf("all whole numbers:\n\t");

  for (size_t i = 0; i < pline.wholes.used; ++i) {
    if (pline.wholes.val[i].sign == PLUS)
      printf("+");
    else
      printf("-");

    printf("%llu, ", pline.wholes.val[i].abs);
  }

  printf("\nall real numbers:\n\t");

  for (size_t i = 0; i < pline.reals.used; ++i)
    printf("%f, ", pline.reals.val[i]);

  printf("\nall nans:\n\t");

  for (size_t i = 0; i < pline.nans.used; ++i)
    printf("'%s', ", pline.nans.val[i]);

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

PLine init_pline(size_t line_num)
{
  PLine pline;
  pline.line_num = line_num;
  pline.well_formed = true;

  pline.wholes.len = INIT_ARR_SIZE;
  pline.wholes.used = 0;
  pline.wholes.val = (Whole*) malloc(INIT_ARR_SIZE * sizeof(Whole));

  pline.reals.len = INIT_ARR_SIZE;
  pline.reals.used = 0;
  pline.reals.val = (double*) malloc(INIT_ARR_SIZE * sizeof(double));

  pline.nans.len = INIT_ARR_SIZE;
  pline.nans.used = 0;
  pline.nans.val = (char**) malloc(INIT_ARR_SIZE * sizeof(char*));


  return pline;
}

void free_text(PText text)
{
  for (size_t i = 0; i < text.used; ++i) {
    /* free whole nums */
    free(text.val[i].wholes.val);

    /* free reals */
    free(text.val[i].reals.val);

    /* free nans */
    for (size_t j = 0; j < text.val[i].nans.used; ++j)
      if (text.val[i].nans.val[j])
        free(text.val[i].nans.val[j]);

    free(text.val[i].nans.val);
  }


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
  bool is_sign = (s[0] == '+' || s[0] == '-');
  char* err;

  if (is_sign && strlen(s) == 1)
    return false;

  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  if (is_sign)
    num.abs = strtoull(s + 1, &err, 0);
  else
    num.abs = strtoull(s, &err, 0);

  if (*err != '\0' || errno == ERANGE)
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

  /* check if it is not a misbehaving integer. idk how toooo */
  /* if (num == ceil(num)) {
   *   printf("this double %f is an int in fact\n", num);
   *   return false;
   * } */

  /* nan is a nan as the name specifies */
  if (isnan(num))
    return false;

  if (*err != '\0' || errno == ERANGE)
    return false;
  else {
    add_parsed_real(pline, num);
    return true;
  }

}

void add_parsed_real(PLine* pline, double num)
{
  pline->reals.used++;

  if (pline->reals.used >= pline->reals.len) {
    pline->reals.len = new_len(pline->reals.len);
    pline->reals.val = (double*) realloc(pline->reals.val,
                                         pline->reals.len * sizeof(double));

    if (!pline->reals.val)
      exit(1);
  }

  pline->reals.val[pline->reals.used - 1] = num;
}


bool parse_nan(PLine* pline, const char* s)
{
  add_parsed_nan(pline, s);
  return true;
}

/* !!! this does not work (yet) !!!
 * it is a necessity to strcpy each of those nans and then save them with their
 * own memory. So that the pline->nans stores pointers to *full strings*. */
void add_parsed_nan(PLine* pline, const char* s)
{
  char* new_nan;

  pline->nans.used++;

  if (pline->nans.used >= pline->nans.len) {
    pline->nans.len = new_len(pline->nans.len);
    pline->nans.val = (char**) realloc(pline->nans.val,
                                       pline->nans.len * sizeof(char*));

    if (!pline->nans.val)
      exit(1);
  }

  new_nan = pline->nans.val[pline->nans.used - 1];
  new_nan = (char*) malloc(strlen(s) + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  pline->nans.val[pline->nans.used - 1] = new_nan;
  /* pline->nans.val[pline->nans.used - 1] = (char*) malloc(strlen(s) + 1);
   * strcpy(pline->nans.val[pline->nans.used - 1], s); */
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
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    }
  }

  return true;
}

