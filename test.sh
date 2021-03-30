#!/bin/bash

# kolory terminalowe
GREEN="\e[1;32m"
RED="\e[1;31m"
DEFAULT="\e[0m"

# tablica na błędy
all_errors=()

# traceback obsługi błędów/sukcesów
function ok {
    printf "\t%s ${GREEN}ok!${DEFAULT}\n" "$1"
}

function bad {
    if [ $# -gt 1 ]; then
        all_errors+=("$2")
    fi
    printf "\t%s ${RED}bad :(${DEFAULT}\n" "$1"
}

# bezpieczna wersja mktemp działająca zarazem linuksowo jak i BSDowo
function tempmk {
    if [ $# -gt 0 ]; then
        mktemp --tmpdir "$1.XXXXXX" 2>/dev/null || mktemp -t "$1"
    else
        mktemp --tmpdir 2>/dev/null || mktemp -t tmp
    fi
}

# szereg sprawdzianów poprawności wywołania
if [ $# -lt 2 ]; then
    echo test.sh: za mało argumentów.
    echo poprawne wywołanie: ./test.sh prog dir
    exit 1
fi

# execa zapisuję wersje z prawdziwymi ścieżkami, aby każda działała (naiwne
# podejście typu "./prog" wywaliłoby się, gdyby ktoś dał ścieżkę absolutną,
# a nie względną)
prog="$1"
real_prog=$(realpath "$prog")

dir="${2%/}"

if [ ! -f "$prog" ]; then
    echo test.sh: brak takiego pliku "$prog".....
    exit 1
fi
if [ ! -x "$prog" ]; then
    echo test.sh: plik "$prog" nie jest wykonywalny....
    exit 1
fi
if [ ! -d "$dir" ]; then
    echo test.sh: brak takiego katalogu "$dir".....
    exit 1
fi


# tymczasowe pliki zbiorniki na stdout i stderr
tmpout=$(tempmk tmpout) && tmperr=$(tempmk tmperr) ||
        (echo błąd w tworzeniu pliku tymczasowego; exit 1)

echo smktempowałem "$tmpout" i "$tmperr" jak coś
echo ---------------------------------

for f in "$dir"/*.in; do
    echo sprawdzam "$f"
    "$real_prog" <"$f" >"$tmpout" 2>"$tmperr"

    if [ $? -eq 1 ]; then
        printf "\tprogram %s zakończył się awaryjnie kodem ${RED}1${DEFAULT}\n"\
               "$prog"
        echo -e '\tpomijam go więc w dalszych rozważaniach'
        continue
    fi
    
    if cmp --silent "${f%in}out" "$tmpout"; then
        ok out
    else
        bad out "${f%in}out"
    fi
    if cmp --silent "${f%in}err" "$tmperr"; then
        ok err
    else
        bad err "${f%in}err"
    fi

    # test pamięci, strasznie powolny. ustawi exit code na 123 w razie błędu
    valgrind --leak-check=full --error-exitcode=123 --exit-on-first-error=yes \
             --errors-for-leak-kinds=all --quiet \
             "$real_prog" <"$f" >/dev/null 2>&1
    
    if [ $? -eq 123 ]; then
        bad valgrind "${f%.in} -- valgrind"
    else
        ok valgrind
    fi
done

rm "$tmpout" "$tmperr"

# podsumowanief
if [ ! "${#all_errors[@]}" -eq 0 ]; then
    echo -------------------------------
    echo -n wszystkie błędne testy:
    bad
    for err in "${all_errors[@]}"; do
        printf "\t%s\n" "${err}"
    done
else
    echo --------------------
    echo -n wszystkie testy
    ok
fi
