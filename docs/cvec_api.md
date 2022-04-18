# STC [cvec](../include/stc/cvec.h): Vector
![Vector](pics/vector.jpg)

A **cvec** is a sequence container that encapsulates dynamic size arrays.

The storage of the vector is handled automatically, being expanded and contracted as needed. Vectors usually occupy more space than static arrays, because more memory is allocated to handle future growth. This way a vector does not need to reallocate each time an element is inserted, but only when the additional memory is exhausted. The total amount of allocated memory can be queried using *cvec_X_capacity()* function. Extra memory can be returned to the system via a call to *cvec_X_shrink_to_fit()*.

Reallocations are usually costly operations in terms of performance. The *cvec_X_reserve()* function can be used to eliminate reallocations if the number of elements is known beforehand.

See the c++ class [std::vector](https://en.cppreference.com/w/cpp/container/vector) for a functional description.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_tag       // defaults to i_val
#include <stc/cvec.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

`i_valdrop` may be defined instead of `i_valdrop` (or `i_keydrop`) for all non-map containers.

## Methods

```c
cvec_X              cvec_X_init(void);
cvec_X              cvec_X_with_size(size_t size, i_val null);
cvec_X              cvec_X_with_capacity(size_t size);
cvec_X              cvec_X_clone(cvec_X vec);

void                cvec_X_clear(cvec_X* self);
void                cvec_X_copy(cvec_X* self, cvec_X other);
bool                cvec_X_reserve(cvec_X* self, size_t cap);
bool                cvec_X_resize(cvec_X* self, size_t size, i_val null);
void                cvec_X_shrink_to_fit(cvec_X* self);
void                cvec_X_swap(cvec_X* a, cvec_X* b);
void                cvec_X_drop(cvec_X* self);                                               // destructor

bool                cvec_X_empty(cvec_X vec);
size_t              cvec_X_size(cvec_X vec);
size_t              cvec_X_capacity(cvec_X vec);

const cvec_X_value* cvec_X_at(const cvec_X* self, size_t idx);
const cvec_X_value* cvec_X_get(const cvec_X* self, i_valraw raw);                            // return NULL if not found
cvec_X_value*       cvec_X_get_mut(cvec_X* self, i_valraw raw);                              // get mutable value
cvec_X_iter         cvec_X_find(const cvec_X* self, i_valraw raw);
cvec_X_iter         cvec_X_find_in(cvec_X_iter i1, cvec_X_iter i2, i_valraw raw);
                    // On sorted vectors:
cvec_X_iter         cvec_X_bsearch(const cvec_X* self, i_valraw raw);                        // at elem == raw, else end
cvec_X_iter         cvec_X_lower_bound(const cvec_X* self, i_valraw raw);                    // at first elem >= raw, else end
cvec_X_iter         cvec_X_bsearch_in(cvec_X_iter i1, cvec_X_iter i2, i_valraw raw, cvec_X_iter* lower_bound);

cvec_X_value*       cvec_X_front(const cvec_X* self);
cvec_X_value*       cvec_X_back(const cvec_X* self);

cvec_X_value*       cvec_X_push(cvec_X* self, i_val value);
cvec_X_value*       cvec_X_emplace(cvec_X* self, i_valraw raw);
cvec_X_value*       cvec_X_push_back(cvec_X* self, i_val value);                             // alias for push
cvec_X_value*       cvec_X_emplace_back(cvec_X* self, i_valraw raw);                         // alias for emplace

void                cvec_X_pop(cvec_X* self);
void                cvec_X_pop_back(cvec_X* self);                                           // alias for pop

cvec_X_iter         cvec_X_insert(cvec_X* self, size_t idx, i_val value);                    // move value 
cvec_X_iter         cvec_X_insert_n(cvec_X* self, size_t idx, const i_val[] arr, size_t n);  // move n values
cvec_X_iter         cvec_X_insert_at(cvec_X* self, cvec_X_iter it, i_val value);             // move value 
cvec_X_iter         cvec_X_insert_range_p(cvec_X* self, i_val* pos,
                                          const i_val* p1, const i_val* p2);

cvec_X_iter         cvec_X_emplace_n(cvec_X* self, size_t idx, const i_valraw[] arr, size_t n);
cvec_X_iter         cvec_X_emplace_at(cvec_X* self, cvec_X_iter it, i_valraw raw);
cvec_X_iter         cvec_X_emplace_range(cvec_X* self, cvec_X_iter it,                       // will clone
                                         cvec_X_iter it1, cvec_X_iter it2);
cvec_X_iter         cvec_X_emplace_range_p(cvec_X* self, i_val* pos, 
                                           const i_val* p1, const i_val* p2);

cvec_X_iter         cvec_X_erase_n(cvec_X* self, size_t idx, size_t n);
cvec_X_iter         cvec_X_erase_at(cvec_X* self, cvec_X_iter it);
cvec_X_iter         cvec_X_erase_range(cvec_X* self, cvec_X_iter it1, cvec_X_iter it2);

void                cvec_X_sort(cvec_X* self);
void                cvec_X_sort_range(cvec_X_iter i1, cvec_X_iter i2,
                                      int(*cmp)(const i_val*, const i_val*));

cvec_X_iter         cvec_X_begin(const cvec_X* self);
cvec_X_iter         cvec_X_end(const cvec_X* self);
void                cvec_X_next(cvec_X_iter* iter);

cvec_X_raw          cvec_X_value_toraw(cvec_X_value* pval);
cvec_X_value        cvec_X_value_clone(cvec_X_value val);
```

## Types

| Type name          | Type definition                   | Used to represent...   |
|:-------------------|:----------------------------------|:-----------------------|
| `cvec_X`           | `struct { cvec_X_value* data; }`  | The cvec type          |
| `cvec_X_value`     | `i_val`                           | The cvec value type    |
| `cvec_X_raw`       | `i_valraw`                        | The raw value type     |
| `cvec_X_iter`      | `struct { cvec_X_value* ref; }`   | The iterator type      |

## Examples
```c
#define i_val int
#include <stc/cvec.h>

#include <stdio.h>

int main()
{
    // Create a vector containing integers
    c_auto (cvec_int, vec)
    {
        // Add two integers to vector
        cvec_int_push(&vec, 25);
        cvec_int_push(&vec, 13);

        // Append a set of numbers
        c_apply(v, cvec_int_push(&vec, v), int, {7, 5, 16, 8});

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
    }
}
```
Output:
```
initial: 25 13 7 5 16 8
sorted: 5 7 8 13 16 25
```
### Example 2
```c
#include <stc/cstr.h>

#define i_val_str
#include <stc/cvec.h>

int main() {
    cvec_str names = cvec_str_init();
    cvec_str_emplace(&names, "Mary");
    cvec_str_emplace(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    cstr tmp = cstr_from_fmt("%d elements so far", cvec_str_size(names));

    // emplace() will not compile if adding a new cstr type. Use push_back():
    cvec_str_push(&names, tmp); // tmp is moved to names, do not drop() it.

    printf("%s\n", names.data[1].str); // Access the second element

    c_foreach (i, cvec_str, names)
        printf("item: %s\n", i.ref->str);
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
#include <stc/cstr.h>

typedef struct {
    cstr name; // dynamic string
    int id;
} User;

int User_cmp(const User* a, const User* b) {
    int c = strcmp(a->name.str, b->name.str);
    return c != 0 ? c : a->id - b->id;
}
void User_drop(User* self) {
    cstr_drop(&self->name);
}
User User_clone(User user) {
    user.name = cstr_clone(user.name);
    return user;
}

// Declare a memory managed, clonable vector of users.
// Note that cvec_u_emplace_back() will clone input:
#define i_val User
#define i_cmp User_cmp
#define i_valdrop User_drop
#define i_valfrom User_clone
#define i_tag u
#include <stc/cvec.h>

int main(void) {
    cvec_u vec = cvec_u_init();
    cvec_u_push(&vec, (User) {cstr_new("admin"), 0});
    cvec_u_push(&vec, (User) {cstr_new("joe"), 1});

    cvec_u vec2 = cvec_u_clone(vec);
    c_foreach (i, cvec_u, vec2)
        printf("%s: %d\n", i.ref->name.str, i.ref->id);

    c_drop(cvec_u, &vec, &vec2); // cleanup
}
```
