exe=''
if [ "$OS" = "Windows_NT" ] ; then exe=".exe" ; fi
clang -x c++ -DNDEBUG -I../../../include -O3 -o deque_benchmark$exe   deque_benchmark.cpp -lstdc++
clang -x c++ -DNDEBUG -I../../../include -O3 -o list_benchmark$exe  list_benchmark.cpp -lstdc++
clang -x c++ -DNDEBUG -I../../../include -O3 -o hmap_benchmark$exe   hmap_benchmark.cpp -lstdc++
clang -x c++ -DNDEBUG -I../../../include -O3 -o smap_benchmark$exe  smap_benchmark.cpp -lstdc++
clang -x c++ -DNDEBUG -I../../../include -O3 -o vec_benchmark$exe   vec_benchmark.cpp -lstdc++

c='Win-Clang-16.0.5'
./deque_benchmark$exe $c
./list_benchmark$exe $c
./hmap_benchmark$exe $c
./smap_benchmark$exe $c
./vec_benchmark$exe $c
