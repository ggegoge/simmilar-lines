#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

/* stałe: zakres akceptowalnych znaków ascii, whitespace oraz znaczniki kluczowe
 * dla systemów liczbowych będących przedmiotem naszych rozważań*/
#define MIN_WORD_ASCII 33
#define MAX_WORD_ASCII 126
#define WHITE " \t\n\v\f\r"

#define OCT_MARKER '0'
#define HEX_MARKER 'x'

static ParsedLine init_pline(size_t line_num);
static void free_line(ParsedLine line);
static bool check_line(char** s, size_t line_num, size_t line_len);
static void parse(ParsedLine* pline, const char* word);

ParsedLine parse_line(char* line, size_t line_num, size_t line_len)
{
  ParsedLine pline;
  char* word = NULL;

  /* sprawdzian poprawności linii. w przypadku jej poprawności zaczynam podział
   * na słowa wraz ze sprawdzianem (nie)pustości. */
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
  ParsedLine pline = { .well_formed = true, .line_num = line_num };
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
  /* err jako argument dla funkcji strto* wskazuje pierwszy niewczytany znak
   * wczyt poprawny --> *err == '\0' */
  char* err;
  /* może być znak na początku, ale po nim cyfra. zał, że s != "", więc można
   * bezpiecznie zajrzeć pod s[0] i s[1] (najwyżej będzie tam \0) */
  bool has_sign = (s[0] == '+' || s[0] == '-') && isdigit(s[1]);

  if (s[0] == '-')
    num.sign = MINUS;
  else
    num.sign = PLUS;

  errno = 0;

  /* zgodnie z forum przed liczbami 8kowymi i 16kowymi nie pojawi się znak */
  if (has_sign)
    num.abs = strtoull(s + 1, &err, 10);
  else {
    num.abs = strtoull(s, &err, 0);

    /* liczba zaczynające się od 0 są przez strtoull traktowane jako ósemkowe
     * nawet jeśli po nich następują cyfry >=8, zwraca dla nich błędnie 0 */
    if (s[0] == OCT_MARKER && *err != '\0')
      num.abs = strtoull(s, &err, 10);
  }

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
  if (errno == ERANGE || s[1] == HEX_MARKER || (s[1] != '\0' &&
      s[2] == HEX_MARKER))
    return false;

  errno = 0;
  num = strtod(s, &err);

  if (*err != '\0' || errno == ERANGE || isnan(num))
    return false;

  /* sprawdzam, czy to nie jest int w przebraniu floata: floor(x) == x
   * i zapisuję tę liczbę jako Whole w takim wypadku */
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
 * Alokacja pamięci pod string s i następne dodanie go do plinijki jako nan. */
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
 * Szybki sprawdzian poprawności znaku -- znak jest ok, jeśli jest to whitespace
 * lub należy do odpowiedniego zakresu */
static inline bool correct_char(char c)
{
  return isspace(c) || (c >= MIN_WORD_ASCII && c <= MAX_WORD_ASCII);
}

/**
 * Sprawdzian zakresu znakow w line_num-tej linii s długości line_len, error
 * w przypadku nieprawidłowości. Do tego lekka normalizacja -- wszystkie duże
 * litery (poprawne) zostają zmienione ma małe. Zwraca informację o poprawności
 * danej linii. */
static bool check_line(char** s, size_t line_num, size_t line_len)
{
  for (size_t i = 0; i < line_len; ++i) {
    if (!correct_char((*s)[i])) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    } else
      (*s)[i] = tolower((*s)[i]);
  }

  return true;
}
