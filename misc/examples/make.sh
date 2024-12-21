#!/bin/sh

#set -e

if [ "$(uname)" = 'Linux' ]; then
    sanitize='-fsanitize=address -fsanitize=undefined -fsanitize-trap'
    clibs='-lm' # -pthread
    oflag='-o '
fi
common="-std=c99 -Wpedantic -Wall -Werror"
#cc=clang; cflags="-s -O3 $common"
cc=gcc; cflags="-s -O3 $common"
#cc=gcc; cflags="-g $sanitize $common"
#cc=gcc; cflags="-x c++ -std=c++17 -O2 -s -DSTC_IMPLEMENT -Di_import"
#cc=tcc; cflags="-std=c99"
#cc=cl; cflags="-nologo -O2 -MD -W3 -c -std:c11 -wd4003"
#cc=cl; cflags="-nologo -c -TP -std:c++20 -wd4003 -DSTC_IMPLEMENT -Di_import"

if [ "$cc" = "cl" ]; then
    oflag='/Fe:'
else
    oflag='-o '
fi

run=0
if [ "$1" = '-h' -o "$1" = '--help' ]; then
  echo usage: runall.sh [-run] [compiler + options]
  exit
fi
if [ "$1" = '-run' ]; then
  run=1
  shift
fi
if [ ! -z "$1" ] ; then
    comp="$@"
else
    comp="$cc $cflags"
fi

INC=
#INC=-I../../include
#INC=-I../../../stcsingle
#CPATH=
if [ $run = 0 ] ; then
    for i in */*.c ; do
        out=$(basename $i .c).exe
        #out=$(dirname $i)/$(basename $i .c).exe
        #echo $comp -I../../../stcsingle $i $clibs $oflag$out
        echo $comp $INC $i $clibs $oflag$out
        $comp $INC $i $clibs $oflag$out -lstc
    done
else
    for i in */*.c ; do
        out=$(basename $i .c).exe
        #out=$(dirname $i)/$(basename $i .c).exe
        echo $comp $INC $i $clibs $oflag$out
        $comp $INC $i $clibs $oflag$out -lstc
        if [ -f $out ]; then ./$out; fi
    done
fi

#rm -f a.out *.o *.obj # *.exe
