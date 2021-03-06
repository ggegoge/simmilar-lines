#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "compare.h"

/* komparatory. Zasadniczo:
 *   cmp(a, b) = if a < b then -1 else if a > b then 1 else 0,
 * Porównując rzecz złożoną (np tablicę, struktury) pierwsza różnica decyduje
 * o porządku między elementami -- quasi leksykograficznie porządkuję.
 * Wyjątek: cmp_pline_stable nigdy nie zwraca równości ponieważ chodzi w nim
 * o zachowanie porządku indeksów w grupie -- jeśli podobne są wiersze 1, 3 i 5,
 * to byłoby naszym życzeniem, aby w tej kolejności były w naszym outpucie.
 * Takie fajne komparacje typu `(a > b) - (a < b)` wziąłem stąd:
 * https://www.gnu.org/software/libc/manual/html_node/Comparison-Functions.html
 * */

/**
 * Funkcja porównujące dwie liczby a i b całkowite typu Whole.
 * Najpierw po znaku (- < +), później po wartości absolutnej (w przypadku znaku
 * MINUS porządek się obraca). */
static int cmp_whole(Whole n1, Whole n2)
{
  if (n1.sign == n2.sign) {
    if (n1.sign == MINUS)
      return (n1.abs < n2.abs) - (n1.abs > n2.abs);
    else
      return (n1.abs > n2.abs) - (n1.abs < n2.abs);
  } else
    return (n1.sign == MINUS && n2.sign == PLUS) ? -1 : 1;
}

static int cmp_real(double n1, double n2)
{
  return (n1 > n2) - (n1 < n2);
}

int cmp_pline(const void* l1, const void* l2)
{
  ParsedLine pl1 = *(ParsedLine*)l1;
  ParsedLine pl2 = *(ParsedLine*)l2;
  int cmp;

  if (pl1.pwords.used != pl2.pwords.used)
    return (pl1.pwords.used < pl2.pwords.used) ? -1 : 1;

  for (size_t i = 0; i < pl1.pwords.used; ++i) {
    /* pierwsza różnica decyduje ponieważ zakładam, że linie są posortowane */
    if ((cmp = cmp_pword(&pl1.pwords.val[i], &pl2.pwords.val[i])))
      return cmp;
  }

  return 0;
}

int cmp_pline_stable(const void* l1, const void* l2)
{
  int cmp;
  size_t line_num1 = ((ParsedLine*)l1)->line_num;
  size_t line_num2 = ((ParsedLine*)l2)->line_num;

  if ((cmp = cmp_pline(l1, l2)))
    return cmp;

  /* w przypadku równości chcemy, aby był zachowany oryginalny porządek danych
   * wyznaczany przez numery wierszy */
  return line_num1 < line_num2 ? -1 : 1;
}

int cmp_pword(const void* p1, const void* p2)
{
  ParsedWord pw1 = *(ParsedWord*)p1;
  ParsedWord pw2 = *(ParsedWord*)p2;

  if (pw1.class == pw2.class) {
    switch (pw1.class) {
    case WHOLE:
      return cmp_whole(pw1.whole, pw2.whole);

    case REAL:
      return cmp_real(pw1.real, pw2.real);

    case NEITHER:
      return strcmp(pw1.nan, pw2.nan);

    default:
      return 0;
    }
  } else
    return pw1.class - pw2.class;
}

int cmp_size_t_p(const void* a, const void* b)
{
  size_t n1 = **(size_t**)a;
  size_t n2 = **(size_t**)b;
  /* zał., że a i b posortowane --> pierwsze elementy są najmniejsze */
  return (n1 > n2) - (n1 < n2);
}
