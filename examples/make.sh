#!/bin/bash
cc='gcc -s -O2 -Wall -std=c99 -pedantic'
#cc='gcc -x c++ -s -O2 -Wall -std=c++20'
#cc='clang -s -O2 -Wall -std=c99 -pedantic -DSTC_USE_SSO'
#cc='clang'
#cc='clang -c -DSTC_HEADER'
#cc='cl -O2 -nologo -W2 -MD'
#cc='cl -nologo -TP'
#cc='cl -nologo -std:c11'
libs=''
#oflag='/Fe:'
oflag='-o '
run=0
if [ -z "$OS" ]; then
    libs='-lm -pthread'
fi
if [ "$1" == '-h' -o "$1" == '--help' ]; then
  echo usage: runall.sh [-run] [compiler + options]
  exit
fi
if [ "$1" == '-run' ]; then
  run=1
  shift
fi
if [ ! -z "$1" ] ; then
    cc=$@
fi

if [ $run = 0 ] ; then
    for i in *.c ; do
        echo $cc -I../include $i $libs $oflag$(basename $i .c).exe
        $cc -I../include $i $libs $oflag$(basename $i .c).exe
    done
else
    for i in *.c ; do
        echo $cc -I../include $i $libs
        $cc -I../include $i $libs
        if [ -f $(basename -s .c $i).exe ]; then ./$(basename -s .c $i).exe; fi
        if [ -f ./a.exe ]; then ./a.exe; fi
        if [ -f ./a.out ]; then ./a.out; fi
    done
fi

rm -f a.out *.o *.obj # *.exe
