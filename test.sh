#!/bin/bash

# traceback obsługi błędów/sukcesów
function ok {
    echo -e '\t' $1 ok!
}
function bad {
    echo -e '\t' $1 nie ok :'('
}

if [ $# -lt 2 ]
then
    echo poprawne wywołanie: ./test.sh prog dir
else
    prog=$1
    dir=$2
    tmpout=tmp_out
    tmperr=tmp_err

    rm $tmpout $tmperr 2>/dev/null
    
    for f in $dir/*.in
    do
        echo sprawdzam $f # $(basename $f)
        ./$prog <$f >$tmpout 2>$tmperr
        
        cmp --silent ${f%in}out $tmpout && ok out || bad out
        
        cmp --silent ${f%in}err $tmperr && ok err || bad err
        
        rm $tmpout $tmperr
    done
fi
             
