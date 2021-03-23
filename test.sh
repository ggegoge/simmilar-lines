#!/bin/bash

# traceback obsługi błędów/sukcesów
function ok {
    printf "\t$1\e[1;32m ok!\e[0m\n"
}

function bad {
    if [ $# -gt 1 ]; then
        all_errors[${#all_errors[@]}]=$2
    fi
    printf "\t$1\e[1;31m bad :(\e[0m\n"
}

# bezpieczna wersja mktemp plik działająca zarazem na linuksie jak i OSX
function tempmk {
    if [ $# -gt 0 ]; then
        mktemp --tmpdir "$1.XXXXXX" 2>/dev/null || mktemp -t $1
    else
        mktemp --tmpdir 2>/dev/null || mktemp -t tmp
    fi
}


if [ $# -lt 2 ]; then
    echo test.sh: za mało argumentów.
    echo poprawne wywołanie: ./test.sh prog dir
    exit 1
fi

prog=$1
dir=${2%/}
if [ ! -f $1 ]; then
    echo test.sh: brak takiego pliku $prog.....
    exit 1
fi
if [ ! -d $dir ]; then
    echo test.sh: brak takiego katalogu $dir.....
    exit 1
fi


# pliki na output i err
tmpout=$(tempmk tmpout) && tmperr=$(tempmk tmperr) ||
        (echo błąd w tworzeniu pliku tymczasowego; exit 1)

echo smktempowałem $tmpout i $tmperr jak coś
echo ---------------------------------

for f in $dir/*.in; do
    
    echo sprawdzam $f
    ./$prog <$f >$tmpout 2>$tmperr

    if cmp --silent ${f%in}out $tmpout; then
        ok out
    else
        bad out ${f%in}out
    fi
    if cmp --silent ${f%in}err $tmperr; then
        ok err
    else
        bad err ${f%in}err
    fi

    # test pamięci, strasznie powolny
    valgrind --leak-check=full --error-exitcode=123 ./$prog <$f >/dev/null 2>&1
    if [ $? -eq 123 ]; then
        bad valgrind "${f%in}valgrind" 
    else
        ok valgrind
    fi
done

rm $tmpout $tmperr

# podsumiwanie
if [ $all_errors ]; then
    echo -------------------------------
    echo err -- błędny stderr, out -- błędny stdout, valgrind -- błędy pamięci
    echo -n wszystkie błędne testy:
    bad
    for err in ${all_errors[@]}; do
        echo -e '\t'$err
    done
else
    echo --------------------
    echo -n wszystkie testy
    ok
fi
