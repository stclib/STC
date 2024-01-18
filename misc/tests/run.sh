#!/usr/bin/sh
# tcc compiler: git://repo.or.cz/tinycc.git
# run tests

cd $(dirname $(realpath $0))

if [ -z "$1" ]; then
    tcc -Wall -I../../include *_test.c main.c -run
else
    cc=$1
    $cc -O2 -Wall -I../../include *_test.c main.c -o tests.exe
    ./tests.exe
    rm -f ./tests.exe
fi
