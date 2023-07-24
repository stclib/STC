#!/bin/sh

if [ "$(uname)" = 'Linux' ]; then
    sanitize='-fsanitize=address -fsanitize=undefined -fsanitize-trap'
    clibs='-lm' # -pthread
    oflag='-o '
fi

cc=gcc; cflags="-std=c99 -s -O3 -Wall -Wextra -Wpedantic -Wconversion -Wwrite-strings -Wdouble-promotion -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-implicit-fallthrough -Wno-missing-field-initializers"
#cc=gcc; cflags="-std=c99 -g -Werror -Wfatal-errors -Wpedantic -Wall $sanitize"
#cc=tcc; cflags="-std=c99 -Wall"
#cc=clang; cflags="-std=c99 -s -O3 -Wall -Wextra -Wpedantic -Wconversion -Wwrite-strings -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-implicit-fallthrough -Wno-missing-field-initializers"
#cc=gcc; cflags="-x c++ -std=c++20 -O2 -s -Wall"
#cc=cl; cflags="-nologo -O2 -MD -W3 -wd4003"
#cc=cl; cflags="-nologo -TP -std:c++20 -wd4003"
#cc=cl; cflags="-nologo -std:c11 -wd4003"

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
    for i in */*.c ; do
        out=$(basename $i .c).exe
        #out=$(dirname $i)/$(basename $i .c).exe
        echo $comp -I../../include $i $clibs $oflag$out
        $comp -I../../include $i $clibs $oflag$out
    done
else
    for i in */*.c ; do
        echo $comp -I../../include $i $clibs
        $comp -I../../include $i $clibs
        out=$(basename $i .c).exe
        #out=$(dirname $i)/$(basename $i .c).exe
        if [ -f $out ]; then ./$out; fi
    done
fi

#rm -f a.out *.o *.obj # *.exe
