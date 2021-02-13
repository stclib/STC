clang++ -I.. -O3 -o cdeq_benchmark.exe   cdeq_benchmark.cpp
clang++ -I.. -O3 -o clist_benchmark.exe  clist_benchmark.cpp
clang++ -I.. -O3 -o cmap_benchmark.exe   cmap_benchmark.cpp
clang++ -I.. -O3 -o csmap_benchmark.exe  csmap_benchmark.cpp
clang++ -I.. -O3 -o cvec_benchmark.exe   cvec_benchmark.cpp

./cdeq_benchmark.exe Win-Clang-11
./clist_benchmark.exe Win-Clang-11
./cmap_benchmark.exe Win-Clang-11
./csmap_benchmark.exe Win-Clang-11
./cvec_benchmark.exe Win-Clang-11
