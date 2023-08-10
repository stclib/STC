# STC [cvec](../include/stc/cvec.h): Vector
![Vector](pics/vector.jpg)

A **cvec** is a sequence container that encapsulates dynamic size arrays.

The storage of the vector is handled automatically, being expanded and contracted as needed. Vectors usually occupy more space than static arrays, because more memory is allocated to handle future growth. This way a vector does not need to reallocate each time an element is inserted, but only when the additional memory is exhausted. The total amount of allocated memory can be queried using *cvec_X_capacity()* function. Extra memory can be returned to the system via a call to *cvec_X_shrink_to_fit()*.

Reallocations are usually costly operations in terms of performance. The *cvec_X_reserve()* function can be used to eliminate reallocations if the number of elements is known beforehand.

See the c++ class [std::vector](https://en.cppreference.com/w/cpp/container/vector) for a functional description.

## Header file and declaration

```c
#define i_type <t>     // container type name
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_cmp <f>      // three-way compare two i_keyraw*
#define i_use_cmp      // define instead of i_cmp only when i_key is an integral/native-type.
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop defined

#define i_keyraw <t>   // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw

#define i_tag <s>      // alternative typename: cvec_{i_tag}. i_tag defaults to i_key
#include <stc/cvec.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cvec_X              cvec_X_init(void);
cvec_X              cvec_X_with_size(intptr_t size, i_key null);
cvec_X              cvec_X_with_capacity(intptr_t size);
cvec_X              cvec_X_clone(cvec_X vec);

void                cvec_X_clear(cvec_X* self);
void                cvec_X_copy(cvec_X* self, const cvec_X* other);
cvec_X_iter         cvec_X_copy_n(cvec_X* self, intptr_t idx, const i_key* arr, intptr_t n);
bool                cvec_X_reserve(cvec_X* self, intptr_t cap);
bool                cvec_X_resize(cvec_X* self, intptr_t size, i_key null);
void                cvec_X_shrink_to_fit(cvec_X* self);
void                cvec_X_drop(cvec_X* self);                              // destructor

bool                cvec_X_empty(const cvec_X* self);
intptr_t            cvec_X_size(const cvec_X* self);
intptr_t            cvec_X_capacity(const cvec_X* self);

const cvec_X_value* cvec_X_at(const cvec_X* self, intptr_t idx);
const cvec_X_value* cvec_X_get(const cvec_X* self, i_keyraw raw);           // return NULL if not found
cvec_X_value*       cvec_X_at_mut(cvec_X* self, intptr_t idx);              // return mutable at idx
cvec_X_value*       cvec_X_get_mut(cvec_X* self, i_keyraw raw);             // find mutable value
cvec_X_iter         cvec_X_find(const cvec_X* self, i_keyraw raw);
cvec_X_iter         cvec_X_find_in(cvec_X_iter i1, cvec_X_iter i2, i_keyraw raw); // return cvec_X_end() if not found
                    // On sorted vectors:
cvec_X_iter         cvec_X_binary_search(const cvec_X* self, i_keyraw raw); // at elem == raw, else end
cvec_X_iter         cvec_X_lower_bound(const cvec_X* self, i_keyraw raw);   // at first elem >= raw, else end
cvec_X_iter         cvec_X_binary_search_in(cvec_X_iter i1, cvec_X_iter i2,
                                            i_keyraw raw, cvec_X_iter* lower_bound);

cvec_X_value*       cvec_X_front(const cvec_X* self);
cvec_X_value*       cvec_X_back(const cvec_X* self);

cvec_X_value*       cvec_X_push(cvec_X* self, i_key value);
cvec_X_value*       cvec_X_emplace(cvec_X* self, i_keyraw raw);
cvec_X_value*       cvec_X_push_back(cvec_X* self, i_key value);            // alias for push
cvec_X_value*       cvec_X_emplace_back(cvec_X* self, i_keyraw raw);        // alias for emplace

void                cvec_X_pop(cvec_X* self);                               // destroy last element
void                cvec_X_pop_back(cvec_X* self);                          // alias for pop
cvec_X_value        cvec_X_pull(cvec_X* self);                              // move out last element

cvec_X_iter         cvec_X_insert_n(cvec_X* self, intptr_t idx, const i_key arr[], intptr_t n); // move values
cvec_X_iter         cvec_X_insert_at(cvec_X* self, cvec_X_iter it, i_key value);  // move value 
cvec_X_iter         cvec_X_insert_uninit(cvec_X* self, intptr_t idx, intptr_t n); // return iter at idx 

cvec_X_iter         cvec_X_emplace_n(cvec_X* self, intptr_t idx, const i_keyraw raw[], intptr_t n);
cvec_X_iter         cvec_X_emplace_at(cvec_X* self, cvec_X_iter it, i_keyraw raw);

cvec_X_iter         cvec_X_erase_n(cvec_X* self, intptr_t idx, intptr_t n);
cvec_X_iter         cvec_X_erase_at(cvec_X* self, cvec_X_iter it);
cvec_X_iter         cvec_X_erase_range(cvec_X* self, cvec_X_iter it1, cvec_X_iter it2);

void                cvec_X_sort(cvec_X* self);
void                cvec_X_sort_range(cvec_X_iter i1, cvec_X_iter i2,
                                      int(*cmp)(const i_key*, const i_key*));

cvec_X_iter         cvec_X_begin(const cvec_X* self);
cvec_X_iter         cvec_X_end(const cvec_X* self);
void                cvec_X_next(cvec_X_iter* iter);
cvec_X_iter         cvec_X_advance(cvec_X_iter it, size_t n);

bool                cvec_X_eq(const cvec_X* c1, const cvec_X* c2); // equality comp.
cvec_X_value        cvec_X_value_clone(cvec_X_value val);
cvec_X_raw          cvec_X_value_toraw(const cvec_X_value* pval);
cvec_X_raw          cvec_X_value_drop(cvec_X_value* pval);
```

## Types

| Type name          | Type definition                   | Used to represent...   |
|:-------------------|:----------------------------------|:-----------------------|
| `cvec_X`           | `struct { cvec_X_value* data; }`  | The cvec type          |
| `cvec_X_value`     | `i_key`                           | The cvec value type    |
| `cvec_X_raw`       | `i_keyraw`                        | The raw value type     |
| `cvec_X_iter`      | `struct { cvec_X_value* ref; }`   | The iterator type      |

## Examples
```c
#define i_key int
#include <stc/cvec.h>

#include <stdio.h>

int main(void)
{
    // Create a vector containing integers
    cvec_int vec = {0};

    // Add two integers to vector
    cvec_int_push(&vec, 25);
    cvec_int_push(&vec, 13);

    // Append a set of numbers
    c_forlist (i, int, {7, 5, 16, 8})
        cvec_int_push(&vec, *i.ref);

    printf("initial:");
    c_foreach (k, cvec_int, vec) {
        printf(" %d", *k.ref);
    }

    // Sort the vector
    cvec_int_sort(&vec);

    printf("\nsorted:");
    c_foreach (k, cvec_int, vec) {
        printf(" %d", *k.ref);
    }
    cvec_int_drop(&vec);
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
#include <stc/cstr.h>

#define i_key_str
#include <stc/cvec.h>

int main(void) {
    cvec_str names = cvec_str_init();

    cvec_str_emplace(&names, "Mary");
    cvec_str_emplace(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    cstr tmp = cstr_from_fmt("%d elements so far", cvec_str_size(names));

    // cvec_str_emplace() only accept const char*, so use push():
    cvec_str_push(&names, tmp); // tmp is "moved" to names (must not be dropped).

    printf("%s\n", cstr_str(&names.data[1])); // Access second element

    c_foreach (i, cvec_str, names)
        printf("item: %s\n", cstr_str(i.ref));

    cvec_str_drop(&names);
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
#include <stc/cstr.h>

typedef struct {
    cstr name; // dynamic string
    int id;
} User;

int User_cmp(const User* a, const User* b) {
    int c = strcmp(cstr_str(&a->name), cstr_str(&b->name));
    return c ? c : a->id - b->id;
}
void User_drop(User* self) {
    cstr_drop(&self->name);
}
User User_clone(User user) {
    user.name = cstr_clone(user.name);
    return user;
}

// Declare a managed, clonable vector of users.
#define i_type UVec
#define i_keyclass User // User is a "class" as it has _cmp, _clone and _drop functions.
#include <stc/cvec.h>

int main(void) {
    UVec vec = {0};
    UVec_push(&vec, (User){cstr_lit("mary"), 0});
    UVec_push(&vec, (User){cstr_lit("joe"), 1});
    UVec_push(&vec, (User){cstr_lit("admin"), 2});

    UVec vec2 = UVec_clone(vec);

    c_foreach (i, UVec, vec2)
        printf("%s: %d\n", cstr_str(&i.ref->name), i.ref->id);

    c_drop(UVec, &vec, &vec2); // cleanup
}
```
