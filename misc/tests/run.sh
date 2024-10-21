#!/usr/bin/sh
# tcc compiler: git://repo.or.cz/tinycc.git
# run tests

cd $(dirname $(realpath $0))
cc=gcc
if [ ! -z "$1" ]; then
    cc=$1
fi

$cc -O2 -Wall -I../../include *_test.c main.c -o tests.exe -lstc
./tests.exe
rm -f ./tests.exe
