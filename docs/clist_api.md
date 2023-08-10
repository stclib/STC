# STC [clist](../include/stc/clist.h): Forward List
![List](pics/list.jpg)

The **clist** container supports fast insertion and removal of elements from anywhere in the container.
Fast random access is not supported.

Unlike the c++ class *std::forward_list*, **clist** has API similar to ***std::list***, and also supports
*push_back()* (**O**(1) time). It is still implemented as a singly-linked list. A **clist** object
occupies only one pointer in memory, and like *std::forward_list* the length of the list is not stored.
All functions have **O**(1) complexity, apart from *clist_X_count()* and *clist_X_find()* which are **O**(*n*),
and *clist_X_sort()* which is **O**(*n* log(*n*)).

***Iterator invalidation***: Adding, removing and moving the elements within the list, or across several lists
will invalidate other iterators currently refering to these elements and their immediate succesive elements.
However, an iterator to a succesive element can both be dereferenced and advanced. After advancing, it is 
in a fully valid state. This implies that if `clist_X_insert(&L, clist_X_advance(it,1), x)` and
`clist_X_erase_at(&L, clist_X_advance(it,1))` are used consistently, only iterators to erased elements are invalidated.

See the c++ class [std::list](https://en.cppreference.com/w/cpp/container/list) for similar API and
[std::forward_list](https://en.cppreference.com/w/cpp/container/forward_list) for a functional description.

## Header file and declaration

```c
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>     // clist container type name
#define i_cmp <f>      // three-way compare two i_keyraw*
#define i_use_cmp      // define instead of i_cmp only when i_key is an integral/native-type.
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop defined

#define i_keyraw <t>   // convertion "raw" type (default: {i_key})
#define i_keyto <f>    // convertion func i_key* => i_keyraw
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_tag <s>      // alternative typename: cpque_{i_tag}. i_tag defaults to i_key
#include <stc/clist.h>
```

`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
clist_X             clist_X_init(void);
clist_X             clist_X_clone(clist_X list);

void                clist_X_clear(clist_X* self);
void                clist_X_copy(clist_X* self, const clist_X* other);
void                clist_X_drop(clist_X* self);                                        // destructor

bool                clist_X_empty(const clist_X* list);
intptr_t            clist_X_count(const clist_X* list);                                 // size() in O(n) time

clist_X_value*      clist_X_back(const clist_X* self);
clist_X_value*      clist_X_front(const clist_X* self);

void                clist_X_push_back(clist_X* self, i_key value);                      // note: no pop_back()
void                clist_X_push_front(clist_X* self, i_key value);
void                clist_X_push(clist_X* self, i_key value);                           // alias for push_back()

void                clist_X_emplace_back(clist_X* self, i_keyraw raw);
void                clist_X_emplace_front(clist_X* self, i_keyraw raw);
void                clist_X_emplace(clist_X* self, i_keyraw raw);                       // alias for emplace_back()

clist_X_iter        clist_X_insert_at(clist_X* self, clist_X_iter it, i_key value);     // return iter to new elem
clist_X_iter        clist_X_emplace_at(clist_X* self, clist_X_iter it, i_keyraw raw);

void                clist_X_pop_front(clist_X* self);
clist_X_iter        clist_X_erase_at(clist_X* self, clist_X_iter it);                   // return iter after it
clist_X_iter        clist_X_erase_range(clist_X* self, clist_X_iter it1, clist_X_iter it2);
intptr_t            clist_X_remove(clist_X* self, i_keyraw raw);                        // removes all matches

clist_X             clist_X_split_off(clist_X* self, clist_X_iter i1, clist_X_iter i2); // split off [i1, i2)
clist_X_iter        clist_X_splice(clist_X* self, clist_X_iter it, clist_X* other);     // return updated valid it
clist_X_iter        clist_X_splice_range(clist_X* self, clist_X_iter it,                // return updated valid it
                                         clist_X* other, clist_X_iter it1, clist_X_iter it2);

clist_X_iter        clist_X_find(const clist_X* self, i_keyraw raw);
clist_X_iter        clist_X_find_in(clist_X_iter it1, clist_X_iter it2, i_keyraw raw);
const i_key*        clist_X_get(const clist_X* self, i_keyraw raw);
i_key*              clist_X_get_mut(clist_X* self, i_keyraw raw);

void                clist_X_reverse(clist_X* self);
void                clist_X_sort(clist_X* self);
void                clist_X_sort_with(clist_X* self, int(*cmp)(const clist_X_value*, const clist_X_value*));

// Node API
clist_X_node*       clist_X_get_node(clist_X_value* val);                               // get enclosing node
clist_X_value*      clist_X_push_back_node(clist_X* self, clist_X_node* node);
clist_X_value*      clist_X_insert_after_node(clist_X* self, clist_X_node* ref, clist_X_node* node);
clist_X_node*       clist_X_unlink_after_node(clist_X* self, clist_X_node* ref);        // return unlinked node
clist_X_node*       clist_X_unlink_front_node(clist_X* self);                           // return unlinked node
void                clist_X_erase_after_node(clist_X* self, clist_X_node* node);

clist_X_iter        clist_X_begin(const clist_X* self);
clist_X_iter        clist_X_end(const clist_X* self);
void                clist_X_next(clist_X_iter* it);
clist_X_iter        clist_X_advance(clist_X_iter it, size_t n);                         // return n elements ahead.

bool                clist_X_eq(const clist_X* c1, const clist_X* c2);                   // equality test
clist_X_value       clist_X_value_clone(clist_X_value val);
clist_X_raw         clist_X_value_toraw(const clist_X_value* pval);
void                clist_X_value_drop(clist_X_value* pval);
```

## Types

| Type name           | Type definition                     | Used to represent...                     |
|:--------------------|:------------------------------------|:-----------------------------------------|
| `clist_X`           | `struct { clist_X_node* last; }`                      | The clist type         |
| `clist_X_node`      | `struct { clist_X_node* next; clist_X_value value; }` | The clist node type |
| `clist_X_value`     | `i_key`                                               | The clist element type |
| `clist_X_raw`       | `i_keyraw`                                            | clist raw value type   |
| `clist_X_iter`      | `struct { clist_value *ref; ... }`                    | clist iterator         |

## Example

Interleave *push_front()* / *push_back()* then *sort()*:
```c
#define i_type DList
#define i_key double
#include <stc/clist.h>

#include <stdio.h>

int main(void) {
    DList list = c_init(DList, {10., 20., 30., 40., 50., 60., 70., 80., 90.});

    c_forrange (i, 1, 10) {
        if (i & 1) DList_push_front(&list, (double) i);
        else       DList_push_back(&list, (double) i);
    }

    printf("initial: ");
    c_foreach (i, DList, list)
        printf(" %g", *i.ref);

    DList_sort(&list); // mergesort O(n*log n)

    printf("\nsorted: ");
    c_foreach (i, DList, list)
        printf(" %g", *i.ref);

    DList_drop(&list);
}
```
Output:
```
initial:  9 7 5 3 1 10 20 30 40 50 60 70 80 90 2 4 6 8
sorted:  1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90
```
### Example 2

Use of *erase_at()* and *erase_range()*:
```c
// erasing from clist
#define i_tag i
#define i_key int
#include <stc/clist.h>

#include <stdio.h>

int main(void)
{
    clist_i L = c_init(clist_i, {10, 20, 30, 40, 50});
                                                // 10 20 30 40 50
    clist_i_iter it = clist_i_begin(&L);        // ^
    clist_i_next(&it); 
    it = clist_i_erase_at(&L, it);              // 10 30 40 50
                                                //    ^
    clist_i_iter end = clist_i_end(&L);         //
    clist_i_next(&it);
    it = clist_i_erase_range(&L, it, end);      // 10 30
                                                //       ^
    printf("mylist contains:");
    c_foreach (x, clist_i, L)
        printf(" %d", *x.ref);
    puts("");

    clist_i_drop(&L);
}
```
Output:
```
mylist contains: 10 30
```

### Example 3

Splice `[30, 40]` from *L2* into *L1* before `3`:
```c
#define i_tag i
#define i_key int
#include <stc/clist.h>

#include <stdio.h>

int main(void) {
    clist_i L1 = c_init(clist_i, {1, 2, 3, 4, 5});
    clist_i L2 = c_init(clist_i, {10, 20, 30, 40, 50});

    clist_i_iter i = clist_i_advance(clist_i_begin(&L1), 2);
    clist_i_iter j1 = clist_i_advance(clist_i_begin(&L2), 2), j2 = clist_i_advance(j1, 2);

    clist_i_splice_range(&L1, i, &L2, j1, j2);

    c_foreach (i, clist_i, L1)
        printf(" %d", *i.ref); puts("");
    c_foreach (i, clist_i, L2)
        printf(" %d", *i.ref); puts("");

    c_drop(clist_i, &L1, &L2);
}
```
Output:
```
1 2 30 40 3 4 5
10 20 50
```
