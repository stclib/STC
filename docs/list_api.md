# STC [list](../include/stc/list.h): Forward List
![List](pics/list.jpg)

The **list** container supports fast insertion and removal of elements from anywhere in the container.
Fast random access is not supported.

Unlike the c++ class *std::forward_list*, **list** has API similar to ***std::list***, and also supports
*push_back()* (**O**(1) time). It is still implemented as a singly-linked list. A **list** object
occupies only one pointer in memory, and like *std::forward_list* the length of the list is not stored.
All functions have **O**(1) complexity, apart from *list_X_count()* and *list_X_find()* which are **O**(*n*),
and *list_X_sort()* which is **O**(*n* log(*n*)).

***Iterator invalidation***: Adding, removing and moving the elements within the list, or across several lists
will invalidate other iterators currently refering to these elements and their immediate succesive elements.
However, an iterator to a succesive element can both be dereferenced and advanced. After advancing, it is
in a fully valid state. This implies that if `list_X_insert(&L, list_X_advance(it,1), x)` and
`list_X_erase_at(&L, list_X_advance(it,1))` are used consistently, only iterators to erased elements are invalidated.

See the c++ class [std::list](https://en.cppreference.com/w/cpp/container/list) for similar API and
[std::forward_list](https://en.cppreference.com/w/cpp/container/forward_list) for a functional description.

## Header file and declaration

```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // list container type name (default: list_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop defined

#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_cmp <fn>       // three-way compare two i_keyraw*
#define i_less <fn>      // less comparison. Alternative to i_cmp
#define i_eq <fn>        // equality comparison. Implicitly defined with i_cmp, but not i_less.

#define i_keyraw <t>     // convertion "raw" type (default: {i_key})
#define i_rawclass <t>   // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#include "stc/list.h"
```
- Defining either `i_use_cmp`, `i_less` or `i_cmp` will enable sorting
- **emplace**-functions are only available when `i_keyraw` is implicitly or explicitly defined.
- In the following, `X` is the value of `i_key` unless `i_type` is defined.

## Methods

```c++
list_X          list_X_init(void);

list_X          list_X_clone(list_X list);
void            list_X_copy(list_X* self, list_X other);
void            list_X_take(list_X* self, list_X unowned);                        // take ownership of unowned
list_X          list_X_move(list_X* self);                                        // move
void            list_X_drop(list_X* self);                                        // destructor

void            list_X_clear(list_X* self);

bool            list_X_is_empty(const list_X* list);
isize           list_X_count(const list_X* list);                                 // size() in O(n) time

list_X_iter     list_X_find(const list_X* self, i_keyraw raw);
list_X_iter     list_X_find_in(list_X_iter it1, list_X_iter it2, i_keyraw raw);

const i_key*    list_X_back(const list_X* self);
const i_key*    list_X_front(const list_X* self);

i_key*          list_X_back_mut(list_X* self);
i_key*          list_X_front_mut(list_X* self);

void            list_X_push_back(list_X* self, i_key value);                      // note: no pop_back()
void            list_X_push_front(list_X* self, i_key value);
void            list_X_push(list_X* self, i_key value);                           // alias for push_back()

void            list_X_emplace_back(list_X* self, i_keyraw raw);
void            list_X_emplace_front(list_X* self, i_keyraw raw);
void            list_X_emplace(list_X* self, i_keyraw raw);                       // alias for emplace_back()

list_X_iter     list_X_insert_at(list_X* self, list_X_iter it, i_key value);      // return iter to new elem
list_X_iter     list_X_emplace_at(list_X* self, list_X_iter it, i_keyraw raw);

void            list_X_pop_front(list_X* self);
list_X_iter     list_X_erase_at(list_X* self, list_X_iter it);                    // return iter after it
list_X_iter     list_X_erase_range(list_X* self, list_X_iter it1, list_X_iter it2);
isize           list_X_remove(list_X* self, i_keyraw raw);                        // removes all matches

list_X          list_X_split_off(list_X* self, list_X_iter i1, list_X_iter i2);   // split off [i1, i2)
list_X_iter     list_X_splice(list_X* self, list_X_iter it, list_X* other);       // return updated valid it
list_X_iter     list_X_splice_range(list_X* self, list_X_iter it,                 // return updated valid it
                                    list_X* other, list_X_iter it1, li
                                    st_X_iter it2);

void            list_X_reverse(list_X* self);
void            list_X_sort(list_X* self);
void            list_X_sort_with(list_X* self, int(*cmp)(const i_key*, const i_key*));

// Node API
list_X_node*    list_X_get_node(i_key* val);                                      // get the enclosing node
i_key*          list_X_push_back_node(list_X* self, list_X_node* node);
i_key*          list_X_insert_after_node(list_X* self, list_X_node* ref, list_X_node* node);
list_X_node*    list_X_unlink_after_node(list_X* self, list_X_node* ref);         // return unlinked node
list_X_node*    list_X_unlink_front_node(list_X* self);                           // return unlinked node
void            list_X_erase_after_node(list_X* self, list_X_node* node);

list_X_iter     list_X_begin(const list_X* self);
list_X_iter     list_X_end(const list_X* self);
void            list_X_next(list_X_iter* it);
list_X_iter     list_X_advance(list_X_iter it, size_t n);                        // return n elements ahead.

bool            list_X_eq(const list_X* c1, const list_X* c2);                   // equality test
i_key           list_X_value_clone(i_key val);
list_X_raw      list_X_value_toraw(const i_key* pval);
void            list_X_value_drop(i_key* pval);
```

## Types

| Type name          | Type definition                                     | Used to represent...    |
|:-------------------|:----------------------------------------------------|:------------------------|
| `list_X`           | `struct { list_X_node* last; }`                     | The list type           |
| `list_X_node`      | `struct { list_X_node* next; list_X_value value; }` | The list node type      |
| `list_X_value`     | `i_key`                                             | The list element type |
| `list_X_raw`       | `i_keyraw`                                          | list raw value type   |
| `list_X_iter`      | `struct { list_value *ref; ... }`                   | list iterator          |

## Example

Interleave *push_front()* / *push_back()* then *sort()*:

[ [Run this code](https://godbolt.org/z/Mhf7afcac) ]
```c++
#define i_type DList, double
#define i_use_cmp
#include "stc/list.h"

#include <stdio.h>

int main(void) {
    DList list = c_make(DList, {10., 20., 30., 40., 50., 60., 70., 80., 90.});

    for (c_range(i, 1, 10)) {
        if (i & 1) DList_push_front(&list, (double) i);
        else       DList_push_back(&list, (double) i);
    }

    printf("initial: ");
    for (c_each(i, DList, list))
        printf(" %g", *i.ref);

    DList_sort(&list); // uses quicksort

    printf("\nsorted: ");
    for (c_each(i, DList, list))
        printf(" %g", *i.ref);

    DList_drop(&list);
}
```

### Example 2
Use of *erase_at()* and *erase_range()*:

[ [Run this code](https://godbolt.org/z/9PnPbe9fv) ]
```c++
#include <stdio.h>
#define i_type IList, int
#include "stc/list.h"

int main(void)
{
    IList L = c_make(IList, {10, 20, 30, 40, 50});
                                            // 10 20 30 40 50
    IList_iter it = IList_begin(&L);        // ^
    IList_next(&it);
    it = IList_erase_at(&L, it);            // 10 30 40 50
                                            //    ^
    IList_iter end = IList_end(&L);         //
    IList_next(&it);
    it = IList_erase_range(&L, it, end);    // 10 30
                                            //       ^
    printf("mylist contains:");
    for (c_each(x, IList, L))
        printf(" %d", *x.ref);
    puts("");

    IList_drop(&L);
}
```

### Example 3
Splice {**30**, **40**} from *L2* into *L1* before **3**:

[ [Run this code](https://godbolt.org/z/cvon65ac6) ]
```c++
#include <stdio.h>
#define i_type IList, int
#include "stc/list.h"

int main(void) {
    IList L1 = c_make(IList, {1, 2, 3, 4, 5});
    IList L2 = c_make(IList, {10, 20, 30, 40, 50});

    for (c_items(k, IList, {L1, L2})) {
        for (c_each(i, IList, *k.ref))
            printf(" %d", *i.ref);
        puts("");
    }
    puts("");
    IList_iter i = IList_advance(IList_begin(&L1), 2);
    IList_iter j1 = IList_advance(IList_begin(&L2), 2), j2 = IList_advance(j1, 2);

    IList_splice_range(&L1, i, &L2, j1, j2);

    for (c_items(k, IList, {L1, L2})) {
        for (c_each(i, IList, *k.ref))
            printf(" %d", *i.ref);
        puts("");
    }

    c_drop(IList, &L1, &L2);
}
```
