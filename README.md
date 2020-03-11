# C99Containers

Introduction
------------
Typesafe, efficient, generic C99 containers: c_String, c_Vector and c_Hashmap

Headers only library with the most useful data structures: string, dynamic vector/stack, and map/assosiative array.

The map is using open hashing with a novel probing strategy (fibonacci sequence), which is as efficient as quadratic probing, but has none of its limitations (max half full table, and prime number table length only requirements).

The library has an intuitive and straight forward API, and is fully type safe. It uses "overloadable macros", to simplify usage.

Usage
-----
Simple c_Vector of 64bit ints:
```
#include "c_vector.h"
c_declare_Vector(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
  c_Vector_ix bignums = c_vector_initializer; // use c_vector_ix_init(); if initializing after declaration.
  c_vector_ix_reserve(&bignums, 100);
  for (size_t i = 0; i<100; ++i)
    c_vector_ix_push(&bignums, i * i * i);
  c_vector_ix_pop(&bignums); // erase the last

  uint64_t value;
  for (size_t i = 0; i < c_vector_size(bignums); ++i)
    value = bignums.data[i];
  c_vector_ix_destroy(&bignums);
}
```
CVector of CString:
```
#include "c_string.h"
#include "c_vector.h"
c_declare_Vector(cs, c_String, c_string_destroy); // supply inline destructor of values

int main() {
  c_Vector_cs names = c_vector_initializer;
  c_vector_cs_push(&names, c_string_make("Mary"));
  c_vector_cs_push(&names, c_string_make("Joe"));
  printf("%s\n", names.data[1].str); // Access the string char*
  c_vector_cs_destroy(&names);
}
```
Simple c_Hashmap, int -> int:
```
#include "c_hashmap.h"
c_declare_Hashmap(ii, int, int);

int main() {
  c_Hashmap_ii nums = c_hashmap_initializer;
  c_hashmap_ii_put(&nums, 8, 64);
  c_hashmap_ii_put(&nums, 11, 121);
  printf("%d\n", c_hashmap_ii_get(nums, 8)->value);
  c_hashmap_ii_destroy(&nums);
}
```
Simple c_Hashmap, c_String -> int:
```
#include "c_string.h"
#include "c_hashmap.h"
c_declare_Hashmap_stringkey(si, int); // Shorthand macro for the general c_declare_Maphash expansion.
// c_String keys are "magically" managed internally, although c_Hashmap is ignorant of c_String.

int main() {
  c_Hashmap_si nums = c_hashmap_initializer;
  c_hashmap_si_put(&nums, "Hello", 64);
  c_hashmap_si_put(&nums, "Groovy", 121);
  c_hashmap_si_put(&nums, "Groovy", 200); // overwrite previous
  // iterate the map:
  for (c_hashmap_si_iter_t i = c_hashmap_si_begin(nums); i.item != c_hashmap_si_end(nums).item; i = c_hashmap_si_next(i))
    printf("%s: %d\n", i.item->key.str, i.item->value);
  // or rather use the short form:
  c_foreach (i, c_hashmap_si, nums)
    printf("%s: %d, changed: %s\n", i.item->key.str, i.item->value, i.item->changed ? "yes" : "no");

  c_hashmap_si_destroy(&nums);
}
```
c_Hashmap, with c_String -> c_String. Temporary c_String values are created by "make", and moved to the container.
```
#include "c_string.h"
#include "c_hashmap.h"
c_declare_Hashmap_stringkey(ss, c_String, c_string_destroy); 

int main() {
  c_Hashmap_ss table = c_hashmap_initializer;
  c_hashmap_ss_put(&table, "Make", c_string_make("my"));
  c_hashmap_ss_put(&table, "Sunny", c_string_make("day"));
  printf("Sunny: %s\n", c_hashmap_ss_get(table, "Sunny")->value.str);
  c_hashmap_ss_erase(&table, "Make");
  printf("size %d\n", c_hashmap_size(table));
  c_hashmap_ss_destroy(&table); // frees key and value c_Strings, and hash table (c_Vector).
}
```
