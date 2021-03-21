#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "array.h"
#include "parse.h"

static ParsedLine init_pline(size_t);
static void free_line(ParsedLine);
static bool check_line(char** s, size_t line_num, size_t line_len);
static void parse(ParsedLine* pline, const char* word);


ParsedLine parse_line(char* line, size_t line_num, size_t line_len)
{
  ParsedLine pline;
  char* word = NULL;
  char* delims = " \t\n\v\f\r";

  /* sprawdzian zakresu, w przypadku dobrego zakresu z inicjalizacją
   * funkcji bibliotecznej strtok dzielącej na słowa względem wskazanych
   * podzielników (delims) -- brak pierwszego słowa oznacza pustość linijki. */
  if (!check_line(&line, line_num, line_len) || !(word = strtok(line, delims))) {
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
 * Funkcja próbująca sparsować słowo w stringu @s i następnie dodać je do linii
 * @pline. Zwracana wartość boolowska mówi czy wczytanie zakończyło się
 * powodzeniem. */
static bool parse_whole(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  Whole num;
  char* err;
  /* musi być znak na początku, a jednocześnie nie może być ++... lub +-... itd */
  bool is_sign = (s[0] == '+' || s[0] == '-') && s[1] != '+' && s[1] != '-';

  /* goły znak to nei liczba */
  if (is_sign && s[1] == '\0')
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
  if (strcmp(s, "0x") != 0 && (*err != '\0' || errno == ERANGE)) {
    /* liczba za duża, ale np zakończona kropką również ustawi ERANGE,
     * ale niekoniecznie chcemy ją odrzucić w przyszłości (tj. może być ok
     * jako liczba rzeczywista) */
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
      array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY);

    array_append(&pline->pwords, sizeof(ParsedWord), &pword);
    return true;
  }
}

/**
 * Funkcja dualna do parse_whole. Wczytanie liczby rzeczywistej ze stringu @s
 * i następne zapisanie jej w @pline; zwraca status udania tej operacji. */
static bool parse_real(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  double num;
  unsigned long long rounded;
  Whole whole_num;
  char* err;

  /* nie chcę tu łapać za dużych intów z notacji intowej, sprawdzam czy wywołany
   * wcześniej parse_whole nie ustawił ostrzeżenia. Also: strtod nie ma opcji
   * specyfikacji systemu liczb, zatem odrzucone hexy typu +0x... -0x...
   * muszę ręcznie odrzucać po małpiemu (do tego brak liczb zmiennoprzeciwnkowych
   * w notacji heksadecymalnej według forum). */
  if (errno == ERANGE || s[1] == 'x' || s[2] == 'x')
    return false;

  errno = 0;
  num = strtod(s, &err);

  /* czy poprawna rzeczywista */
  if (*err != '\0' || errno == ERANGE || isnan(num))
    return false;

  rounded = (unsigned long long) fabs(num);

  /* sprawdźmy, czy to nie jest int w przebraniu floata */
  if (isfinite(num) && fabs(num) == rounded && fabs(num) <= ULLONG_MAX) {
    whole_num.abs = rounded;

    if (num >= 0)
      whole_num.sign = PLUS;
    else
      whole_num.sign = MINUS;

    pword.class = WHOLE;
    pword.whole = whole_num;

    if (pline->pwords.len == 0)
      array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY);

    array_append(&pline->pwords, sizeof(ParsedWord), &pword);
    return true;
  }

  /* ^nie udało się wykryć liczby całkowitej, jest to więc liczba rzeczywista */
  pword.class = REAL;
  pword.real = num;

  if (pline->pwords.len == 0)
    array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY);

  array_append(&pline->pwords, sizeof(ParsedWord), &pword);
  return true;
}

/**
 * Alokacja pamięci pod string @s i następnie dodanie go do plinijki @pline */
static void new_parsed_nan(ParsedLine* pline, const char* s)
{
  ParsedWord pword;
  size_t word_len = strlen(s);
  char* new_nan = (char*) malloc(word_len + 1);

  if (!new_nan)
    exit(1);

  strcpy(new_nan, s);
  pword.class = NEITHER;
  pword.nan = new_nan;

  if (pline->pwords.len == 0)
    array_init(&pline->pwords, sizeof(ParsedWord), SMALL_ARRAY);

  array_append(&pline->pwords, sizeof(ParsedWord), &pword);
}

/**
 * Próba sparsowania @word na 3 możliwe sposoby. */
static void parse(ParsedLine* pline, const char* word)
{
  if (!(parse_whole(pline, word) || parse_real(pline, word)))
    new_parsed_nan(pline, word);
}

/**
 * Sprawdzian zakresu znakow w @line_num-tej linii @s długości @line_len, a do
 * tego lekka normalizacja -- wszystkie duże litery zostają zmienione ma małe.
 * Zwraca informację o jej poprawności. */
static bool check_line(char** s, size_t line_num, size_t line_len)
{
  for (size_t i = 0; i < line_len; ++i) {
    if (!isspace((*s)[i]) && ((*s)[i] < MIN_WORD_ASCII ||
                              (*s)[i] > MAX_WORD_ASCII)) {
      fprintf(stderr, "ERROR %lu\n", line_num);
      return false;
    } else
      (*s)[i] = tolower((*s)[i]);
  }

  return true;
}
