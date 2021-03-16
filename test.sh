#!/bin/bash

# traceback obsługi błędów/sukcesów
function ok {
    printf "\t$*\e[1;32m ok!\e[0m\n"
}
function bad {
    printf "\t$*\e[1;31m bad :(\e[0m\n"
}

if [ $# -lt 2 ]
then
    echo za mało argumentów. poprawne wywołanie: ./test.sh prog dir
    exit 1;
fi

prog=$1
dir=${2%/}
if [ ! -f $1 ]
then
    echo brak takiego pliku $prog.....
    exit 1
fi
if [ ! -d $dir ]
then
    echo brak takiego katalogu $dir.....
    exit 1
fi
tmpout=tmp_out
tmperr=tmp_err

rm $tmpout $tmperr 2>/dev/null

for f in $dir/*.in
do
    echo sprawdzam $f
    ./$prog <$f >$tmpout 2>$tmperr
    
    cmp --silent ${f%in}out $tmpout && ok out || bad out
    
    cmp --silent ${f%in}err $tmperr && ok err || bad err
    
    rm $tmpout $tmperr
done
