# STC [vec](../include/stc/vec.h): Vector
![Vector](pics/vector.jpg)

A **vec** is a sequence container that encapsulates dynamic size arrays.

The storage of the vector is handled automatically, being expanded and contracted as needed. Vectors usually occupy more space than static arrays, because more memory is allocated to handle future growth. This way a vector does not need to reallocate each time an element is inserted, but only when the additional memory is exhausted. The total amount of allocated memory can be queried using *vec_X_capacity()* function. Extra memory can be returned to the system via a call to *vec_X_shrink_to_fit()*.

Reallocations are usually costly operations in terms of performance. The *vec_X_reserve()* function can be used to eliminate reallocations if the number of elements is known beforehand.

See the c++ class [std::vector](https://en.cppreference.com/w/cpp/container/vector) for a functional description.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // container type name (default: vec_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines vec_X_value
#define i_cmp <f>        // three-way compare two i_keyraw*
#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_keydrop <f>    // destroy value func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key
#define i_keyto <f>      // convertion func i_key* => i_keyraw

#define i_tag <s>        // alternative typename: vec_{i_tag}. i_tag defaults to i_key
#include "stc/vec.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
vec_X               vec_X_init(void);
vec_X               vec_X_with_size(intptr_t size, i_key null);
vec_X               vec_X_with_capacity(intptr_t size);
vec_X               vec_X_clone(vec_X vec);

void                vec_X_clear(vec_X* self);
void                vec_X_copy(vec_X* self, const vec_X* other);
vec_X_iter          vec_X_copy_n(vec_X* self, intptr_t idx, const i_key* arr, intptr_t n);
bool                vec_X_reserve(vec_X* self, intptr_t cap);
bool                vec_X_resize(vec_X* self, intptr_t size, i_key null);
void                vec_X_shrink_to_fit(vec_X* self);
void                vec_X_drop(vec_X* self);                              // destructor

bool                vec_X_is_empty(const vec_X* self);
intptr_t            vec_X_size(const vec_X* self);
intptr_t            vec_X_capacity(const vec_X* self);

const vec_X_value*  vec_X_at(const vec_X* self, intptr_t idx);
const vec_X_value*  vec_X_get(const vec_X* self, i_keyraw raw);           // return NULL if not found
vec_X_value*        vec_X_at_mut(vec_X* self, intptr_t idx);              // return mutable at idx
vec_X_value*        vec_X_get_mut(vec_X* self, i_keyraw raw);             // find mutable value
vec_X_iter          vec_X_find(const vec_X* self, i_keyraw raw);
vec_X_iter          vec_X_find_in(vec_X_iter i1, vec_X_iter i2, i_keyraw raw); // return vec_X_end() if not found
void                vec_X_sort(vec_X* self);                              // qsort() from stdlib.h
vec_X_value*        vec_X_bsearch(const vec_X* self, i_key value);        // bsearch() wrapper.

vec_X_value*        vec_X_front(const vec_X* self);
vec_X_value*        vec_X_back(const vec_X* self);

vec_X_value*        vec_X_push(vec_X* self, i_key value);
vec_X_value*        vec_X_emplace(vec_X* self, i_keyraw raw);
vec_X_value*        vec_X_push_back(vec_X* self, i_key value);            // alias for push
vec_X_value*        vec_X_emplace_back(vec_X* self, i_keyraw raw);        // alias for emplace

void                vec_X_pop(vec_X* self);                               // destroy last element
void                vec_X_pop_back(vec_X* self);                          // alias for pop
vec_X_value         vec_X_pull(vec_X* self);                              // move out last element

vec_X_iter          vec_X_insert_n(vec_X* self, intptr_t idx, const i_key arr[], intptr_t n); // move values
vec_X_iter          vec_X_insert_at(vec_X* self, vec_X_iter it, i_key value);  // move value
vec_X_iter          vec_X_insert_uninit(vec_X* self, intptr_t idx, intptr_t n); // return iter at idx

vec_X_iter          vec_X_emplace_n(vec_X* self, intptr_t idx, const i_keyraw raw[], intptr_t n);
vec_X_iter          vec_X_emplace_at(vec_X* self, vec_X_iter it, i_keyraw raw);

vec_X_iter          vec_X_erase_n(vec_X* self, intptr_t idx, intptr_t n);
vec_X_iter          vec_X_erase_at(vec_X* self, vec_X_iter it);
vec_X_iter          vec_X_erase_range(vec_X* self, vec_X_iter it1, vec_X_iter it2);


vec_X_iter          vec_X_begin(const vec_X* self);
vec_X_iter          vec_X_end(const vec_X* self);
void                vec_X_next(vec_X_iter* iter);
vec_X_iter          vec_X_advance(vec_X_iter it, size_t n);

bool                vec_X_eq(const vec_X* c1, const vec_X* c2); // equality comp.
vec_X_value         vec_X_value_clone(vec_X_value val);
vec_X_raw           vec_X_value_toraw(const vec_X_value* pval);
vec_X_raw           vec_X_value_drop(vec_X_value* pval);
```

## Types

| Type name         | Type definition                  | Used to represent...  |
|:------------------|:---------------------------------|:----------------------|
| `vec_X`           | `struct { vec_X_value* data; }`  | The vec type          |
| `vec_X_value`     | `i_key`                          | The vec value type    |
| `vec_X_raw`       | `i_keyraw`                       | The raw value type    |
| `vec_X_iter`      | `struct { vec_X_value* ref; }`   | The iterator type     |

## Examples
```c
#define i_key int
#define i_use_cmp // enable sorting/searhing using default <, == operators
#include "stc/vec.h"

#include <stdio.h>

int main(void)
{
    // Create a vector containing integers
    vec_int vec = {0};

    // Add two integers to vector
    vec_int_push(&vec, 25);
    vec_int_push(&vec, 13);

    // Append a set of numbers
    c_foritems (i, int, {7, 5, 16, 8})
        vec_int_push(&vec, *i.ref);

    printf("initial:");
    c_foreach (k, vec_int, vec) {
        printf(" %d", *k.ref);
    }

    // Sort the vector
    vec_int_sort(&vec);

    printf("\nsorted:");
    c_foreach (k, vec_int, vec) {
        printf(" %d", *k.ref);
    }
    vec_int_drop(&vec);
}
```
Output:
```
initial: 25 13 7 5 16 8
sorted: 5 7 8 13 16 25
```
### Example 2
```c
#define i_implement
#include "stc/cstr.h"

#define i_key_cstr
#include "stc/vec.h"

int main(void) {
    vec_cstr names = {0};

    vec_cstr_emplace(&names, "Mary");
    vec_cstr_emplace(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    cstr tmp = cstr_from_fmt("%d elements so far", vec_cstr_size(names));

    // vec_cstr_emplace() only accept const char*, so use push():
    vec_cstr_push(&names, tmp); // tmp is "moved" to names (must not be dropped).

    printf("%s\n", cstr_str(&names.data[1])); // Access second element

    c_foreach (i, vec_cstr, names)
        printf("item: %s\n", cstr_str(i.ref));

    vec_cstr_drop(&names);
}
```
Output:
```
Jake
item: Mary
item: Jake
item: 2 elements so far
```
### Example 3

Container with elements of structs:
```c
#define i_implement
#include "stc/cstr.h"

typedef struct {
    cstr name; // dynamic string
    int id;
} User;

int User_cmp(const User* a, const User* b) {
    int c = strcmp(cstr_str(&a->name), cstr_str(&b->name));
    return c ? c : a->id - b->id;
}

User User_clone(User user) {
    user.name = cstr_clone(user.name);
    return user;
}

void User_drop(User* self) {
    cstr_drop(&self->name);
}

// Declare a managed, clonable vector of users.
#define i_type Users
#define i_keyclass User  // User is a "class" and binds the _clone, _drop, and _cmp functions.
#define i_use_cmp        // Sorting/searching a vec is only enabled by either directly specifying an i_cmp function
                         // or by defining i_use_cmp (i_cmp is then indirectly specified through i_keyclass,
                         // or it is assumed that i_key is a built-in type that works with < and == operators).
#include "stc/vec.h"

int main(void) {
    Users users = {0};
    Users_push(&users, (User){cstr_lit("mary"), 0});
    Users_push(&users, (User){cstr_lit("joe"), 1});
    Users_push(&users, (User){cstr_lit("admin"), 2});

    Users users2 = Users_clone(users);
    Users_sort(&users2);

    c_foreach (i, Users, users2)
        printf("%s: %d\n", cstr_str(&i.ref->name), i.ref->id);

    c_drop(Users, &users, &users2); // cleanup
}
```
