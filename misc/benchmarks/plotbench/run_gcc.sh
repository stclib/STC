g++ -DNDEBUG -I../../../include -O3 -o deq_benchmark   deq_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o list_benchmark  list_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o hmap_benchmark  hmap_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o smap_benchmark  smap_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o vec_benchmark   vec_benchmark.cpp

c='Mingw-g++-13.1.0'
./deq_benchmark $c
./list_benchmark $c
./hmap_benchmark $c
./smap_benchmark $c
./vec_benchmark $c
