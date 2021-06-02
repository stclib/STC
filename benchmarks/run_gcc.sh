g++ -I../include -O3 -o cdeq_benchmark.exe   cdeq_benchmark.cpp
g++ -I../include -O3 -o clist_benchmark.exe  clist_benchmark.cpp
g++ -I../include -O3 -o cmap_benchmark.exe   cmap_benchmark.cpp
g++ -I../include -O3 -o csmap_benchmark.exe  csmap_benchmark.cpp
g++ -I../include -O3 -o cvec_benchmark.exe   cvec_benchmark.cpp

c=Mingw-g++-10.30
./cdeq_benchmark.exe $c
./clist_benchmark.exe $c
./cmap_benchmark.exe $c
./csmap_benchmark.exe $c
./cvec_benchmark.exe $c