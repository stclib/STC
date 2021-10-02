exe=''
if [ "$OS" = "Windows_NT" ] ; then exe=".exe" ; fi
clang++ -I../include -O3 -o cdeq_benchmark$exe   cdeq_benchmark.cpp
clang++ -I../include -O3 -o clist_benchmark$exe  clist_benchmark.cpp
clang++ -I../include -O3 -o cmap_benchmark$exe   cmap_benchmark.cpp
clang++ -I../include -O3 -o csmap_benchmark$exe  csmap_benchmark.cpp
clang++ -I../include -O3 -o cvec_benchmark$exe   cvec_benchmark.cpp

c='Win-Clang-12'
./cdeq_benchmark$exe $c
./clist_benchmark$exe $c
./cmap_benchmark$exe $c
./csmap_benchmark$exe $c
./cvec_benchmark$exe $c
