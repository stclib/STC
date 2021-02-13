g++ -I.. -O3 -o cdeq_benchmark.exe   cdeq_benchmark.cpp
g++ -I.. -O3 -o clist_benchmark.exe  clist_benchmark.cpp
g++ -I.. -O3 -o cmap_benchmark.exe   cmap_benchmark.cpp
g++ -I.. -O3 -o csmap_benchmark.exe  csmap_benchmark.cpp
g++ -I.. -O3 -o cvec_benchmark.exe   cvec_benchmark.cpp

./cdeq_benchmark.exe Mingw-g++-9.20
./clist_benchmark.exe Mingw-g++-9.20
./cmap_benchmark.exe Mingw-g++-9.20
./csmap_benchmark.exe Mingw-g++-9.20
./cvec_benchmark.exe Mingw-g++-9.20