#include "array.h"
#include "input.h"
#include "parse.h"
#include "group.h"

/**
 * Wierszarz, RIDMI
 *
 * Program dzieli się na kilka kroków i zagnieżdżonych podkroków. Te główne są
 * dość dobrze wyróżnione w mainie.
 *
 * MODUS OPERANDI:
 *   - inicjalizacja tablicy dynamicznej rodem z modułu `array' przechowującej
 *     sparsowany tekst
 *   - wczytanie tekstu z stdinu (moduł `input')
 *     - moduł input woła bezpośrednio po każdej linii ze stdinu moduł `parse'
 *       i jegoż funkcję `parse_line', która próbuje wczytać słowa z linii
 *       i dodać je do wspomnianej tablicy `ParsedText'
 *     - w razie błędnej linii nie wczytuje jej
 *     - po tej części programu tablica `text' powinna być stosownie wypełniona
 *   - moduł group i jego eksportowa funkcja `write_groups' znajdują podobne
 *     wiersze i wypluwają na stdout
 *     - każda linia ma tablicę swoich słów -- `group' sortuje linie wewnątrz
 *       nich samych, aby podobne miały te same tablicowe reprezentacje
 *     - następnie dochodzi do sortu wszystkich linii po tychże tablicach
 *       wewnętrznych aby z tej postaci wyłuskać duplikaty -- tym samym
 *       zdobywamy poszukiwane tablice podobnych wierszy, na których nam tutaj
 *       zależy
 *       - wszelkie sortowanie wymaga funkcji porządkujących: moduł `compare'
 *
 *  ZŁOŻONOŚĆ
 *  Ze względu na wspomniane sortowania złożoność można szacować asymptotą od
 *  góry O(k log k), gdzie k to liczba znaków na wejściu.
 *
 *  MODUŁY
 *    (a) `array' -- implementacja dynamicznych tablic, chyba z tej części
 *        jestem najbardziej dumny. przewijają się w prawie każdym aspekcie
 *        rozwiązania, a jednocześnie liczba alokacji jest chyba przyzwoita
 *    (b) `input'
 *    (c) `parse'
 *    (d) `group'
 *    (c) `compare'
 */

int main(void)
{
  ParsedText text;

  array_init(&text, sizeof(ParsedLine), 0);
  read_text(&text);
  write_groups(text);
  free_text(text);

  return 0;
}
