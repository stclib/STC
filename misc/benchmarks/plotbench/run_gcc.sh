exe=''
if [ "$OS" = "Windows_NT" ] ; then exe=".exe" ; fi
g++ -DNDEBUG -I../../../include -O3 -o deque_benchmark$exe   deque_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o list_benchmark$exe  list_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o hmap_benchmark$exe   hmap_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o smap_benchmark$exe  smap_benchmark.cpp
g++ -DNDEBUG -I../../../include -O3 -o vec_benchmark$exe   vec_benchmark.cpp

c='Mingw-g++-13.1.0'
./deque_benchmark$exe $c
./list_benchmark$exe $c
./hmap_benchmark$exe $c
./smap_benchmark$exe $c
./vec_benchmark$exe $c
