# C99Containers

Introduction
------------
Typesafe, efficient, generic C99 containers: CString, CVector and CMap

Headers only library with the most useful data structures: string, dynamic vector/stack, and map/assosiative array.

The map is using open hashing with a novel probing strategy (fibonacci sequence), which is as efficient as quadratic probing, but has none of its limitations (max half full table, and prime number table length only requirements).

The library has an intuitive and straight forward API, and is fully type safe. It uses "overloadable macros", to simplify usage.

Usage
-----
Simple CVector of 64bit ints:
```
#include "cvector.h"
declare_CVector(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
  CVector_ix bignums = cvector_initializer; // use cvector_ix_init(); if initializing after declaration.
  cvector_ix_reserve(&bignums, 100);
  for (size_t i = 0; i<100; ++i)
    cvector_ix_push(&bignums, i * i * i);
  cvector_ix_pop(&bignums); // erase the last

  uint64_t value;
  for (size_t i = 0; i < cvector_size(bignums); ++i)
    value = bignums.data[i];
  cvector_ix_destroy(&bignums);
}
```
CVector of CString:
```
#include "cstring.h"
#include "cvector.h"
declare_CVector(cs, CString, cstring_destroy); // supply inline destructor of values

int main() {
  CVector_cs names = cvector_initializer;
  cvector_cs_push(&names, cstring_make("Mary"));
  cvector_cs_push(&names, cstring_make("Joe"));
  printf("%s\n", names.data[1].str); // Access the string char*
  cvector_cs_destroy(&names);
}
```
Simple CMap, int -> int:
```
#include "cmap.h"
declare_CMap(ii, int, int);

int main() {
  CMap_ii nums = cmap_initializer;
  cmap_ii_put(&nums, 8, 64);
  cmap_ii_put(&nums, 11, 121);
  printf("%d\n", cmap_ii_get(nums, 8)->value);
  cmap_ii_destroy(&nums);
}
```
Simple CMap with CString keys -> int values:
```
#include "cstring.h"
#include "cmap.h"
declare_CMap_StringKey(si, int); // Just a shorthand macro for the general declare call.
// Keys strings are "magically" managed internally, although CMap is ignorant of CString.

int main() {
  CMap_si nums = cmap_initializer;
  cmap_si_put(&nums, "Hello", 64);
  cmap_si_put(&nums, "Groovy", 121);
  cmap_si_put(&nums, "Groovy", 200); // overwrite previous
  // iterate the map:
  for (cmap_si_iter_t i = cmap_si_begin(nums); i.item != cmap_si_end(nums).item; i = cmap_si_next(i))
    printf("%s: %d\n", i.item->key.str, i.item->value);
  // or rather use the short form:
  cforeach (i, cmap_si, nums)
    printf("%s: %d, changed: %s\n", i.item->key.str, i.item->value, i.item->changed ? "yes" : "no");

  cmap_si_destroy(&nums);
}
```
CMap with CString keys -> CString values. Temp. CStrings are created by "make", and moved to the container.
```
#include "cstring.h"
#include "cmap.h"
declare_CMap_StringKey(ss, CString, cstring_destroy); 

int main() {
  CMap_ss table = cmap_initializer;
  cmap_ss_put(&table, "Hello", cstring_make("Goodbye"));
  cmap_ss_put(&table, "Groovy", cstring_make("Shaky"));
  printf("Groovy: %s\n", cmap_ss_get(table, "Groovy")->value.str);
  cmap_ss_erase(&table, "Hello");
  printf("size %d\n", cmap_size(table));
  cmap_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}
```
