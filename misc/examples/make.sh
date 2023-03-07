#!/bin/sh

if [ "$(uname)" = 'Linux' ]; then
    sanitize='-fsanitize=address'
    clibs='-lm' # -pthread
    oflag='-o '
fi

cc=gcc; cflags="-s -O3 -std=c99 -Wconversion -Wpedantic -Wall -Wsign-compare -Wwrite-strings"
#cc=gcc; cflags="-g -std=c99 -Werror -Wfatal-errors -Wpedantic -Wall $sanitize"
#cc=tcc; cflags="-Wall -std=c99"
#cc=clang; cflags="-s -O2 -std=c99 -Werror -Wfatal-errors -Wpedantic -Wall -Wno-unused-function -Wsign-compare -Wwrite-strings"
#cc=gcc; cflags="-x c++ -s -O2 -Wall -std=c++20"
#cc=g++; cflags="-x c++ -s -O2 -Wall"
#cc=cl; cflags="-O2 -nologo -W3 -MD"
#cc=cl; cflags="-nologo -TP"
#cc=cl; cflags="-nologo -std:c11"

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

if [ $run = 0 ] ; then
    for i in *.c ; do
        echo $comp -I../../include $i $clibs $oflag$(basename $i .c).exe
        $comp -I../../include $i $clibs $oflag$(basename $i .c).exe
    done
else
    for i in *.c ; do
        echo $comp -I../../include $i $clibs
        $comp -I../../include $i $clibs
        if [ -f $(basename -s .c $i).exe ]; then ./$(basename -s .c $i).exe; fi
        if [ -f ./a.exe ]; then ./a.exe; fi
        if [ -f ./a.out ]; then ./a.out; fi
    done
fi

rm -f a.out *.o *.obj # *.exe
