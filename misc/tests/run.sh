# run tests
# tcc compiler: git://repo.or.cz/tinycc.git

cd $(dirname $(realpath $0))

if [ -z "$1" ]; then
    cc=gcc
else
    cc=$1
fi
if [ "$cc" == "tcc" ]; then
    tcc -O2 -Wall -I../../include *_test.c main.c -run
else
    $cc -O2 -Wall -I../../include *_test.c main.c -o tests.exe
    ./tests.exe
    rm -f ./tests.exe
fi
