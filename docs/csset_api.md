# STC [csset](../include/stc/csset.h): Sorted Set
![Set](pics/sset.jpg)

A **csset** is an associative container that contains a sorted set of unique objects of type *Key*. Sorting is done using the key comparison function *keyCompare*. Search, removal, and insertion operations have logarithmic complexity. **csset** is implemented as an AA-tree.

See the c++ class [std::set](https://en.cppreference.com/w/cpp/container/set) for a functional description.

## Header file and declaration

```c
#define i_tag       // defaults to i_key name
#define i_key       // key: REQUIRED
#define i_cmp       // three-way compare two i_keyraw* : REQUIRED IF i_keyraw is a non-integral type
#define i_keyraw    // convertion "raw" type - defaults to i_key
#define i_keyfrom   // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto     // convertion func i_key* => i_keyraw - defaults to plain copy
#define i_keydel    // destroy key func - defaults to empty destruct
#include <stc/csset.h>
```
`X` should be replaced by the value of ***i_tag*** in all of the following documentation.

## Methods

```c
csset_X             csset_X_init(void);
csset_X             csset_X_clone(csset_x set);

void                csset_X_clear(csset_X* self);
void                csset_X_swap(csset_X* a, csset_X* b);
void                csset_X_del(csset_X* self);                                                 // destructor

bool                csset_X_empty(csset_X set);
size_t              csset_X_size(csset_X set);

bool                csset_X_contains(const csset_X* self, RawKey rkey);
csset_X_value_t*    csset_X_get(const csset_X* self, RawKey rkey);                              // return NULL if not found
csset_X_iter_t      csset_X_lower_bound(const csset_X* self, RawKey rkey);                      // find closest entry >= rkey
csset_X_iter_t      csset_X_find(const csset_X* self, RawKey rkey);
csset_X_value_t*    csset_X_find_it(const csset_X* self, RawKey rkey, csset_X_iter_t* out);     // return NULL if not found

csset_X_result_t    csset_X_insert(csset_X* self, Key key);
csset_X_result_t    csset_X_emplace(csset_X* self, RawKey rkey);
void                csset_X_emplace_items(csset_X* self, const RawKey arr[], size_t n);

size_t              csset_X_erase(csset_X* self, RawKey rkey);
csset_X_iter_t      csset_X_erase_at(csset_X* self, csset_X_iter_t it);                         // return iter after it
csset_X_iter_t      csset_X_erase_range(csset_X* self, csset_X_iter_t it1, csset_X_iter_t it2); // return updated it2

csset_X_iter_t      csset_X_begin(const csset_X* self);
csset_X_iter_t      csset_X_end(const csset_X* self);
void                csset_X_next(csset_X_iter_t* it);

csset_X_value_t     csset_X_value_clone(csset_X_value_t val);
```

## Types

| Type name            | Type definition                                   | Used to represent...        |
|:---------------------|:--------------------------------------------------|:----------------------------|
| `csset_X`            | `struct { ... }`                                  | The csset type              |
| `csset_X_rawkey_t`   | `i_rawkey`                                        | The raw key type            |
| `csset_X_rawvalue_t` | `i_rawkey`                                        | The raw key type            |
| `csset_X_key_t`      | `i_key`                                           | The key type                |
| `csset_X_value_t`    | `i_key        `                                   | The value: key is immutable |
| `csset_X_result_t`   | `struct { csset_X_value_t* ref; bool inserted; }` | Result of insert/emplace    |
| `csset_X_iter_t`     | `struct { csset_X_value_t *ref; ... }`            | Iterator type               |

## Example
```c
#include <stc/cstr.h>

#define i_key_str
#include <stc/csset.h>

int main ()
{
    csset_str first = csset_str_init(); // empty
    c_var (csset_str, second, {"red", "green", "blue"});
    c_var (csset_str, third, {"orange", "pink", "yellow"});

    csset_str fourth = csset_str_init();
    csset_str_emplace(&fourth, "potatoes");
    csset_str_emplace(&fourth, "milk");
    csset_str_emplace(&fourth, "flour");

    csset_str fifth = csset_str_clone(second);
    c_foreach (i, csset_str, third)
        csset_str_emplace(&fifth, i.ref->str);
    c_foreach (i, csset_str, fourth)
        csset_str_emplace(&fifth, i.ref->str);

    c_del(csset_str, &first, &second, &third, &fourth);

    printf("fifth contains:\n\n");
    c_foreach (i, csset_str, fifth)
        printf("%s\n", i.ref->str);

    csset_str_del(&fifth);
    return 0;
}
```
Output:
```
fifth contains:

blue
flour
green
milk
orange
pink
potatoes
red
yellow
```
