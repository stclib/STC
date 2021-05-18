# STC [csview](../stc/csview.h): String View
![String](pics/string.jpg)

The type **csview** is a string view and can refer to a constant contiguous sequence of char-elements with the first
element of the sequence at position zero. The implementation holds two members: a pointer to constant char and a size.

**csview** is an efficient replacent for `const char*`. It never allocates memory, and therefore need not be destructed.
Its lifetime is limited by the source string storage. It keeps the length of the string, and does not call *strlen()*
when passing it around. It is faster when using`csview` as convertion type (raw) than `const char*` in associative
containers with cstr keys. E.g. prefer `using_cmap_svkey()` over `using_cmap_strkey()`.

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) for a functional
description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#include <stc/csview.h>
```
## Methods

```c
csview        c_sv(const char literal_only[]);                           // shorthand for csview_new()
csview        csview_new(const char literal_only[]);                     // csview from literal, no strlen()
csview        csview_from(const char* str);                              // construct
csview        csview_from_n(const char* str, size_t n);                  // construct
csview        csview_from_s(cstr s);                                     // construct
csview        csview_remove_prefix(csview sv, size_t n);
csview        csview_remove_suffix(csview sv, size_t n);
csview        csview_substr(csview sv, size_t pos, size_t n);
 
size_t        csview_size(csview sv);
size_t        csview_length(csview sv);
bool          csview_empty(csview sv);
 
void          csview_clear(csview* self);

bool          csview_equals(csview sv, csview sv2);
size_t        csview_find(csview sv, csview needle);
bool          csview_contains(csview sv, csview needle);
bool          csview_begins_with(csview sv, csview sub);
bool          csview_ends_with(csview sv, csview sub);

const char*   csview_front(const csview* self);
const char*   csview_back(const csview* self);

csview_iter_t csview_begin(const csview* self);
csview_iter_t csview_end(const csview* self);
void          csview_next(csview_iter_t* it);
```
#### Extended cstr methods
```c
cstr          cstr_from_v(csview sv);
csview        cstr_to_v(const cstr* self);
cstr*         cstr_assign_v(cstr* self, csview sv);
cstr*         cstr_append_v(cstr* self, csview sv);
void          cstr_insert_v(cstr* self, size_t pos, csview sv);
void          cstr_replace_v(cstr* self, size_t pos, size_t len, csview sv);

bool          cstr_equals_v(cstr s, csview sv);
size_t        cstr_find_v(cstr s, csview needle);
bool          cstr_contains_v(cstr s, csview needle);
bool          cstr_begins_with_v(cstr s, csview sub);
bool          cstr_ends_with_v(cstr s, csview sub);
```
#### Helper methods
```c
int           csview_compare_ref(const csview* x, const csview* y);
bool          csview_equals_ref(const csview* x, const csview* y);
uint64_t      csview_hash_ref(const csview* x, size_t ignored);
```
## Types

| Type name         | Type definition                           | Used to represent...     |
|:------------------|:------------------------------------------|:-------------------------|
| `csview`          | `struct { const char *str; size_t size }` | The string view type     |
| `csview_value_t`  | `char`                                    | The string element type  |
| `csview_iter_t`   | `struct { csview_value_t *ref; }`         | csview iterator          |

## Constants and macros

| Name             | Value              | Usage                            |
|:-----------------|:-------------------|:---------------------------------|
| `csview_null`    | same as `c_sv("")` | `sview = csview_null;`           |
| `c_sv(literal)`  | csview constructor | `sview = c_sv("hello, world");`  |
| `csview_PRN(sv)` | printf argument    | `printf("%.*s", csview_PRN(sv));`|

## Container adaptors
```
using_cvec_sv()
using_cdeq_sv()
using_clist_sv()

using_csmap_svkey(X, Mapped)
using_csmap_svkey(X, Mapped, mappedDel)
using_csmap_svkey(X, Mapped, mappedDel, mappedClone)
using_csmap_svkey(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped)
using_csmap_sv()
using_csset_sv()

using_cmap_svkey(X, Mapped)
using_cmap_svkey(X, Mapped, mappedDel)
using_cmap_svkey(X, Mapped, mappedDel, mappedClone)
using_cmap_svkey(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped)
using_cmap_sv()
using_cset_sv()
```

## Example
```c
#include <stc/csview.h>
#include <stc/cvec.h>
#include <stc/cmap.h>

// cmap<cstr, int> with csview as convertion type
using_cmap_svkey(si, int);
// cvec<cstr> with csview as convertion type
using_cvec_sv();

int main()
{
    csview text = c_sv("The length of this literal is evaluated at compile time and stored in csview text.");
    printf("%s\nLength: %zu\n\n", text.str, text.size);

    // cvec of cstr elements, using csview as "emplace" type
    c_var (cvec_sv, vec, {
        c_sv("Element 1"),
        c_sv("Element 2"),
        c_sv("Element 3")
    });
    
    // push constructed cstr directly
    cvec_sv_push_back(&vec, cstr_new("Second last element"));
    // emplace constructs cstr from a csview
    cvec_sv_emplace_back(&vec, c_sv("Last element"));

    c_foreach (i, cvec_sv, vec)
        printf("%s\n", i.ref->str);

    c_var (cmap_si, map, {
        {c_sv("hello"), 100},
        {c_sv("world"), 200}
    });
    cmap_si_emplace(&map, c_sv("gone mad"), 300);

    // cmap_si_get() knows the length of "world" without strlen().
    cmap_si_value_t* v = cmap_si_get(&map, c_sv("world"));
    printf("\n%s: %d\n", v->first.str, v->second);

    cmap_si_del(&map);
    cvec_sv_del(&vec);
}
```
Output:
```
A long and winded literal string
Length: 32

Great
Fantastic
Sensational
Second last element
Last element

world: 200
```
