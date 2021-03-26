#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

/* stałe: zakres akceptowalnych znaków ascii + whitespace */
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126
#define WHITE " \t\n\v\f\r"

static ParsedLine init_pline(size_t);
static void free_line(ParsedLine);
static bool check_line(char** s, size_t line_num, size_t line_len);
static void parse(ParsedLine* pline, const char* word);

ParsedLine parse_line(char* line, size_t line_num, size_t line_len)
{
  ParsedLine pline;
  char* word = NULL;

  /* sprawdzian zakresu. w przypadku dobrego zakresu inicjalizacja
   * podziału na słowa wraz ze sprawdzianem pustości danej linijki. */
  if (!check_line(&line, line_num, line_len) || !(word = strtok(line, WHITE))) {
    pline.well_formed = false;
    return pline;
  }

  pline = init_pline(line_num);

  while (word) {
    parse(&pline, word);
    word = strtok(NULL, WHITE);
  }

  return pline;
}

static ParsedLine init_pline(size_t line_num)
{
  ParsedLine pline;

  pline.line_num = line_num;
  pline.well_formed = true;
  array_init(&pline.pwords, sizeof(ParsedWord), 0);
  return pline;
}

static void free_line(ParsedLine line)
{
  for (size_t i = 0; i < line.pwords.used; ++i)
    if (line.pwords.val[i].class == NEITHER)
      free(line.pwords.val[i].nan);

  free(line.pwords.val);
}

void free_text(ParsedText text)
{
  for (size_t i = 0; i < text.used; ++i)
    free_line(text.val[i]);

  free(text.val);
}

/**
 * Funkcja próbująca sparsować niepuste słowo jako liczbę całkowitą typu Whole
 * i następnie dodać je do linii. Zwracana wartość boolowska mówi, czy wczytanie
 * zakończyło się powodzeniem. */
static bool parse_whole(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  Whole num;
  char* err;
  /* może być znak na początku, ale po nim cyfra. zał, że s != "", więc można
   * bezpiecznie zajrzeć pod s[0] i s[1] (najwyżej będzie tam \0) */
  bool is_sign = (s[0] == '+' || s[0] == '-') && isdigit(s[1]);

  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  errno = 0;

  /* zgodnie z forum przed liczbami 8kowymi i 16kowymi nie pojawi się znak */
  if (is_sign)
    num.abs = strtoull(s + 1, &err, 10);
  else
    num.abs = strtoull(s, &err, 0);

  /* odrzucenie czegoś jako nieliczby(całkowitej) + corner case "0x == 0" */
  if (strcmp(s, "0x") != 0 && (*err != '\0' || errno == ERANGE)) {
    /* liczba za duża, ale np zakończona kropką również ustawi errno == ERANGE,
     * ale niekoniecznie jest to porządane (może to być ok l. rzeczywista),
     * zatem resetujemy errno, aby późniejszy parse_real go nie dojrzał */
    if (errno == ERANGE && *err != '\0')
      errno = 0;

    return false;
  } else {
    /* -0, 0 i +0 zapisuję jako +0 */
    if (num.abs == 0)
      num.sign = PLUS;

    pword.class = WHOLE;
    pword.whole = num;

    if (pline->pwords.len == 0)
      array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY_LENGTH);

    array_append(&pline->pwords, sizeof(ParsedWord), &pword);
    return true;
  }
}

/**
 * Funkcja zupełnie dualna do parse_whole, ale dopuszczalna jest sytuacja,
 * w której w notacji liczb zmiennoprzecinkowych dostaniemy liczbę całkowitą,
 * więc ta funkcja wyłapuje i to \RR \cap \ZZ. */
static bool parse_real(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  double num;
  unsigned long long floored;
  Whole whole_num;
  char* err;

  /* nie chcę tu łapać za dużych intów z notacji intowej, sprawdzam czy wywołany
   * wcześniej parse_whole nie ustawił ostrzeżenia. Do tego: strtod nie ma opcji
   * specyfikacji systemu liczb, zatem hexy 0x... +0x... -0x... muszę ręcznie
   * odrzucać po małpiemu (+ w ifie upewniam się co do bezpiecznosci s[2]) */
  if (errno == ERANGE || s[1] == 'x' || (s[1] != '\0' && s[2] == 'x'))
    return false;

  errno = 0;
  num = strtod(s, &err);

  /* czy poprawna rzeczywista */
  if (*err != '\0' || errno == ERANGE || isnan(num))
    return false;

  /* sprawdzam, czy to nie jest int w przebraniu floata: floor(x) == x */
  floored = (unsigned long long)fabs(num);

  if (isfinite(num) && fabs(num) == floored && fabs(num) <= ULLONG_MAX) {
    whole_num.abs = floored;

    if (num >= 0)
      whole_num.sign = PLUS;
    else
      whole_num.sign = MINUS;

    pword.class = WHOLE;
    pword.whole = whole_num;

    if (pline->pwords.len == 0)
      array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY_LENGTH);

    array_append(&pline->pwords, sizeof(ParsedWord), &pword);
    return true;
  }

  /* ^nie udało się wykryć liczby całkowitej, jest to więc liczba rzeczywista */
  pword.class = REAL;
  pword.real = num;

  if (pline->pwords.len == 0)
    array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY_LENGTH);

  array_append(&pline->pwords, sizeof(ParsedWord), &pword);
  return true;
}

/**
 * Alokacja pamięci pod string @s i następnie dodanie go do plinijki @pline
 * jako nan vel NEITHER. */
static void new_parsed_nan(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  size_t word_len = strlen(s);
  /* alokacja z zapasem na \0 */
  char* new_nan = malloc(word_len + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  pword.class = NEITHER;
  pword.nan = new_nan;

  if (pline->pwords.len == 0)
    array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY_LENGTH);

  array_append(&pline->pwords, sizeof(ParsedWord), &pword);
}

static void parse(ParsedLine* pline, const char* word)
{
  if (!parse_whole(pline, word) && !parse_real(pline, word))
    new_parsed_nan(pline, word);
}

/**
 * Sprawdzian zakresu znakow w @line_num-tej linii @s długości @line_len + error
 * w przypadku nieprawidłowości, a do tego lekka normalizacja -- wszystkie duże
 * litery z zakresu zostają zmienione ma małe. Zwraca informację o poprawności
 * danej linii. */
static bool check_line(char** s, size_t line_num, size_t line_len)
{
  for (size_t i = 0; i < line_len; ++i) {
    /* znak nieprawidłowy jest spoza zakresu i nie jest whitespace'em */
    if (!isspace((*s)[i]) && ((*s)[i] < MIN_WORD_ASCII ||
                              (*s)[i] > MAX_WORD_ASCII)) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    } else
      (*s)[i] = tolower((*s)[i]);
  }

  return true;
}
