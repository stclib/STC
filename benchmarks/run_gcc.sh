g++ -I../include -O3 -o cdeq_benchmark   cdeq_benchmark.cpp
g++ -I../include -O3 -o clist_benchmark  clist_benchmark.cpp
g++ -I../include -O3 -o cmap_benchmark   cmap_benchmark.cpp
g++ -I../include -O3 -o csmap_benchmark  csmap_benchmark.cpp
g++ -I../include -O3 -o cvec_benchmark   cvec_benchmark.cpp

c='Mingw-g++-10.30'
./cdeq_benchmark $c
./clist_benchmark $c
./cmap_benchmark $c
./csmap_benchmark $c
./cvec_benchmark $c