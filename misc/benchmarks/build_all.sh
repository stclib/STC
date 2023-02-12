#!/bin/bash
cc='g++ -I../../include -s -O3 -Wall -pedantic -x c++ -std=c++2a'
#cc='clang++ -I../include -s -O3 -Wall -pedantic -x c++ -std=c++20'
#cc='cl -nologo -I../include -O2 -TP -EHsc -std:c++20'
run=0
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
    for i in *.cpp various/*.c* picobench/*.cpp plotbench/*.cpp ; do
        echo $cc -I../include $i -o $(basename -s .cpp $i).exe
        $cc -I../include $i -o $(basename -s .cpp $i).exe
    done
else
    for i in various/*.c* picobench/*.cpp ; do
        echo $cc -O3 -I../include $i
        $cc -O3 -I../include $i
        if [ -f $(basename -s .c $i).exe ]; then ./$(basename -s .c $i).exe; fi
        if [ -f ./a.exe ]; then ./a.exe; fi
        if [ -f ./a.out ]; then ./a.out; fi
    done
fi

rm -f a.out *.o *.obj # *.exe
