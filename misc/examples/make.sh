#!/bin/sh

if [ "$(uname)" = 'Linux' ]; then
    sanitize='-fsanitize=address -fsanitize=undefined -fsanitize-trap'
    clibs='-lm' # -pthread
    oflag='-o '
fi

cc=gcc; cflags="-std=c99 -s -O3 -Wall -Wextra -Wpedantic -Wconversion -Wwrite-strings -Wdouble-promotion -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-implicit-fallthrough -Wno-missing-field-initializers"
#cc=gcc; cflags="-DSTC_STATIC -std=c99 -g -Werror -Wfatal-errors -Wpedantic -Wall $sanitize"
#cc=tcc; cflags="-DSTC_STATIC -std=c99 -Wall"
#cc=clang; cflags="-DSTC_STATIC -std=c99 -s -O3 -Wall -Wextra -Wpedantic -Wconversion -Wwrite-strings -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-implicit-fallthrough -Wno-missing-field-initializers"
#cc=gcc; cflags="-DSTC_STATIC -x c++ -std=c++20 -O2 -s -Wall"
#cc=cl; cflags="-DSTC_STATIC -nologo -O2 -MD -W3 -wd4003"
#cc=cl; cflags="-DSTC_STATIC -nologo -TP -wd4003"
#cc=cl; cflags="-DSTC_STATIC -nologo -std:c11 -wd4003"

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
