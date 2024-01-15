# git://repo.or.cz/tinycc.git

cd $(dirname $(realpath $0))
tcc -I../../include *_test.c main.c -run
