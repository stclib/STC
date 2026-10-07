#!/bin/sh

#set -e

if [ "$(uname)" = 'Linux' ]; then
    sanitize='-fsanitize=address -fsanitize=undefined -fsanitize-trap'
    clibs='-lm' # -pthread
    oflag='-o '
fi
common="-Wall -Werror"
cc=gcc; cflags="-std=c99 -s -O3 $common"
#cc=clang; cflags="-std=c99 -s -O3 $common"
#cc=gcc; cflags="-std=c99 -g $sanitize $common"
#cc=gcc; cflags="-x c++ -std=c++20 -s -O2 $common"
#cc=tcc; cflags="-std=c11"
#cc=cl; cflags="-nologo -EHsc -O2 -MD -W3 -std:c11 -wd4003"
#cc=cl; cflags="-nologo -EHsc -O2 -MD -TP -std:c++20 -Zc:preprocessor -wd4003"

if [ "$cc" = "cl" ]; then
    oflag="-Fe:"
    stclib="-c" # "../meson_msvc/stc.lib"
elif [ "$cc" = "gcc" -o "$cc" = "clang" -o "$cc" = "tcc" ]; then
    oflag="-o "
    stclib="-lstc" # "../build/Windows_$cc/libstc.a"
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

#INC=
INC=-I../include
#INC=-I../../stcsingle
#CPATH=
for i in */*.c ; do
    out=$(basename $i .c).exe
    #out=$(dirname $i)/$(basename $i .c).exe
    echo $cc $cflags $INC $i $clibs $stclib $oflag$out
    $cc $cflags $INC $i $clibs $stclib $oflag$out
    echo "___________________________________________________________________________________________"
    if [ $run = 1 -a -f $out ]; then ./$out; fi
done

rm -f a.out *.o *.obj # *.exe
