#include "array.h"
#include "input.h"
#include "parse.h"
#include "group.h"

/**
 * Wierszarz, Grzegorz Cichosz, marzec 2021
 *
 * RIDMI
 *  Program dzieli się na kilka kroków i zagnieżdżonych podkroków. Te główne są
 *  dość dobrze wyróżnione w mainie.
 *
 * MODUS OPERANDI
 *   - inicjalizacja tablicy dynamicznej rodem z modułu `array' przechowującej
 *     sparsowany tekst
 *   - wczytanie tekstu z stdinu (moduł `input')
 *     + moduł `input' woła bezpośrednio po każdej linii ze stdinu moduł `parse'
 *       i jegoż funkcję `parse_line', która próbuje wczytać słowa z wiersza
 *       do wspomnianej tablicy
 *   - moduł `group' i funkcja `write_groups' znajdują grupy podobnych
 *     i wypluwają na stdout
 *     + każda linia ma tablicę swoich słów -- `group' sortuje linie wewnątrz
 *       nich samych, aby podobne miały identyczne tablicowe reprezentacje
 *     + następnie sortowanie wszystkich linii po tychże tablicach
 *       wewnętrznych, aby z tej postaci wyłuskać duplikaty (tj podobne wiersze)
 *       - wszelkie sortowanie wymaga funkcji porządkujących z modułu `compare'
 *
 * ZŁOŻONOŚĆ
 *  Ze względu na wspomniane sortowania złożoność można szacować asymptotą od
 *  góry O(k log k), gdzie k to liczba znaków na wejściu.
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
