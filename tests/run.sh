#!/usr/bin/bash
# tcc compiler: git://repo.or.cz/tinycc.git
# run tests

cd $(dirname $(realpath $0))
if [ -z "$CC" ]; then CC=gcc; fi
if [ ! -z "$1" ]; then CC=$1; fi

os=$(uname -s)
if [ "$os" == "Linux" ]; then
    platform=Linux
elif [ "$os" == "Darwin" ]; then
    platform=Mac
else
    platform=Windows
fi

cd ..
make CC=$CC

build/${platform}_${CC}/tests/test_all
echo "OS=$os, CC=$CC"
