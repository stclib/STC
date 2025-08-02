# STC [vec](../include/stc/vec.h): Vector
![Vector](pics/vector.jpg)

A **vec** is a sequence container that encapsulates dynamic size arrays.

The storage of the vector is handled automatically, being expanded and contracted as needed. Vectors usually occupy more space than static arrays, because more memory is allocated to handle future growth. This way a vector does not need to reallocate each time an element is inserted, but only when the additional memory is exhausted. The total amount of allocated memory can be queried using *vec_X_capacity()* function. Extra memory can be returned to the system via a call to *vec_X_shrink_to_fit()*.

Reallocations are usually costly operations in terms of performance. The *vec_X_reserve()* function can be used to eliminate reallocations if the number of elements is known beforehand.

See the c++ class [std::vector](https://en.cppreference.com/w/cpp/container/vector) for a functional description.

## Header file and declaration

```c++
#define T <ct>,<kt>[,<op>] // shorthand for defining T, i_key, i_opt
#define T <ct>           // container type name (default: vec_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

// Use alone or combined with i_keyclass:
#define i_cmpclass <ct>  // Comparison "class". <ct>, aka `raw` defaults to <kt>
                         // Bind <ct>_cmp(),  <ct>_eq(),  <ct>_hash() member functions.

// Override or define when not "class" or "pro" is used:
#define i_keydrop <fn>   // Destroy element func - defaults to empty destruct
#define i_keyclone <fn>  // Clone element func (required when i_keydrop is defined)

#define i_cmp <fn>       // Three-way compare two i_keyraw*
#define i_less <fn>      // Less comparison. Alternative to i_cmp
#define i_eq <fn>        // Equality comparison. Implicitly defined with i_cmp, but not i_less.

#include <stc/vec.h>
```
- Defining either `i_use_cmp`, `i_less` or `i_cmp` will enable sorting, binary_search and lower_bound
- **emplace**-functions are only available when `i_keyraw` is implicitly or explicitly defined.
- In the following, `X` is the value of `i_key` unless `T` is defined.

## Methods

```c++
vec_X           vec_X_init(void);
vec_X           vec_X_with_capacity(isize size);
vec_X           vec_X_with_size(isize size, i_keyraw rawval);
vec_X           vec_X_with_size_uninit(isize size);

vec_X           vec_X_clone(vec_X vec);
void            vec_X_copy(vec_X* self, const vec_X* other);
vec_X_iter      vec_X_copy_to(vec_X* self, isize idx, const i_key* arr, isize n);
vec_X           vec_X_move(vec_X* self);                                    // move
void            vec_X_take(vec_X* self, vec_X unowned);                     // take ownership of unowned
void            vec_X_drop(const vec_X* self);                              // destructor

void            vec_X_clear(vec_X* self);
bool            vec_X_reserve(vec_X* self, isize cap);
bool            vec_X_resize(vec_X* self, isize size, i_key null);
void            vec_X_shrink_to_fit(vec_X* self);

bool            vec_X_is_empty(const vec_X* self);
isize           vec_X_size(const vec_X* self);
isize           vec_X_capacity(const vec_X* self);

vec_X_iter      vec_X_find(const vec_X* self, i_keyraw raw);
vec_X_iter      vec_X_find_in(vec_X_iter i1, vec_X_iter i2, i_keyraw raw);  // return vec_X_end() if not found

const i_key*    vec_X_at(const vec_X* self, isize idx);
const i_key*    vec_X_front(const vec_X* self);
const i_key*    vec_X_back(const vec_X* self);

i_key*          vec_X_at_mut(vec_X* self, isize idx);                       // return mutable at idx
i_key*          vec_X_front_mut(vec_X* self);
i_key*          vec_X_back_mut(vec_X* self);

                // Requires either i_use_cmp, i_cmp or i_less defined:
void            vec_X_sort(vec_X* self);                                    // quicksort from sort.h
isize           vec_X_lower_bound(const vec_X* self, const i_keyraw raw);   // return c_NPOS if not found
isize           vec_X_binary_search(const vec_X* self, const i_keyraw raw); // return c_NPOS if not found

i_key*          vec_X_push(vec_X* self, i_key value);
i_key*          vec_X_push_back(vec_X* self, i_key value);                  // alias for push
i_key*          vec_X_emplace(vec_X* self, i_keyraw raw);
i_key*          vec_X_emplace_back(vec_X* self, i_keyraw raw);              // alias for emplace

void            vec_X_pop(vec_X* self);                                     // destroy last element
void            vec_X_pop_back(vec_X* self);                                // alias for pop
i_key           vec_X_pull(vec_X* self);                                    // move out last element

vec_X_iter      vec_X_insert_n(vec_X* self, isize idx, const i_key arr[], isize n); // move values
vec_X_iter      vec_X_insert_at(vec_X* self, vec_X_iter it, i_key value);   // move value
vec_X_iter      vec_X_insert_uninit(vec_X* self, isize idx, isize n);       // return iter at idx

vec_X_iter      vec_X_emplace_n(vec_X* self, isize idx, const i_keyraw raw[], isize n);
vec_X_iter      vec_X_emplace_at(vec_X* self, vec_X_iter it, i_keyraw raw);

vec_X_iter      vec_X_erase_n(vec_X* self, isize idx, isize n);
vec_X_iter      vec_X_erase_at(vec_X* self, vec_X_iter it);
vec_X_iter      vec_X_erase_range(vec_X* self, vec_X_iter it1, vec_X_iter it2);


vec_X_iter      vec_X_begin(const vec_X* self);
vec_X_iter      vec_X_end(const vec_X* self);
void            vec_X_next(vec_X_iter* iter);
vec_X_iter      vec_X_advance(vec_X_iter it, size_t n);

bool            vec_X_eq(const vec_X* c1, const vec_X* c2); // equality comp.
vec_X_value     vec_X_value_clone(const vec_X* self, vec_X_value val);
vec_X_raw       vec_X_value_toraw(const vec_X_value* pval);
vec_X_raw       vec_X_value_drop(vec_X_value* pval);
```

## Types

| Type name         | Type definition                  | Used to represent...  |
|:------------------|:---------------------------------|:----------------------|
| `vec_X`           | `struct { vec_X_value* data; }`  | The vec type          |
| `vec_X_value`     | `i_key`                          | The vec value type    |
| `vec_X_raw`       | `i_keyraw`                       | The raw value type    |
| `vec_X_iter`      | `struct { vec_X_value* ref; }`   | The iterator type     |

## Examples

[ [Run this code](https://godbolt.org/z/P84zMWro8) ]
```c++
#include <stdio.h>

// enable sorting/searching using default <, == operators
#define T vec_int, int, (c_use_cmp)
#include <stc/vec.h>

int main(void)
{
    // Create a vector containing integers
    vec_int vec = {0};

    // Add two integers to vector
    vec_int_push(&vec, 25);
    vec_int_push(&vec, 13);

    // Append a set of numbers
    for (c_items(i, int, {7, 5, 16, 8}))
        vec_int_push(&vec, *i.ref);

    printf("initial:");
    for (c_each(k, vec_int, vec)) {
        printf(" %d", *k.ref);
    }

    // Sort the vector
    vec_int_sort(&vec);

    printf("\nsorted:");
    for (c_each(k, vec_int, vec)) {
        printf(" %d", *k.ref);
    }
    vec_int_drop(&vec);
}
```
### Example 2

[ [Run this code](https://godbolt.org/z/c7e3q5v14) ]
```c++
#include <stdio.h>
#include <stc/cstr.h>

#define i_keypro cstr
#include <stc/vec.h>

int main(void) {
    vec_cstr names = {0};

    vec_cstr_emplace(&names, "Mary");
    vec_cstr_emplace(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    cstr tmp = cstr_from_fmt("%d elements so far", vec_cstr_size(&names));

    // vec_cstr_emplace() only accept const char*, so use push():
    vec_cstr_push(&names, tmp); // tmp is "moved" to names (must not be dropped).

    printf("%s\n", cstr_str(&names.data[1])); // Access second element

    for (c_each(i, vec_cstr, names))
        printf("item: %s\n", cstr_str(i.ref));

    vec_cstr_drop(&names);
}
```
### Example 3
Container with elements of structs:

[ [Run this code](https://godbolt.org/z/bWEb5vc4K) ]
```c++
#include <stdio.h>
#include <stc/cstr.h>

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
// c_keyclass: User is a "class" and binds the _clone(), _drop(), and _cmp() functions.
// c_use_cmp:  Sorting/searching a vec is only enabled by either directly specifying an
//             i_cmp function or by specifying c_use_cmp. In this case i_cmp is indirectly
//             bound to User_cmp because c_keyclass was specified, otherwise it is assumed that
//             i_key is a built-in type that works with < and == operators).
#define T Users, User, (c_keyclass | c_use_cmp)
#include <stc/vec.h>

int main(void) {
    Users users = {0};
    Users_push(&users, (User){cstr_lit("mary"), 0});
    Users_push(&users, (User){cstr_lit("joe"), 1});
    Users_push(&users, (User){cstr_lit("admin"), 2});

    Users users2 = Users_clone(users);
    Users_sort(&users2);

    for (c_each(i, Users, users2))
        printf("%s: %d\n", cstr_str(&i.ref->name), i.ref->id);

    c_drop(Users, &users, &users2); // cleanup
}
```
