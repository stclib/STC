# STC [csview](../include/stc/csview.h): String View
![String](pics/string.jpg)

The type **csview** is a string view and can refer to a constant contiguous sequence of char-elements with the first
element of the sequence at position zero. The implementation holds two members: a pointer to constant char and a size.

**csview** is an efficient replacent for `const char*`. It never allocates memory, and therefore need not be destructed.
Its lifetime is limited by the source string storage. It keeps the length of the string, and does not call *strlen()*
when passing it around. It is faster when using`csview` as convertion type (raw) than `const char*` in associative
containers with cstr keys. E.g. prefer `using_cmap_svkey()` over `using_cmap_strkey()`.

Note that a **csview** may not be null-terminated, and should therefore be printed the following way: 
`printf("%.*s", csview_arg(sv))`.

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) for a functional
description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#include <stc/csview.h>
```
## Methods

```c
csview        c_lit(const char literal_only[]);                     // csview from literal, no strlen()
csview        c_sv(cstr s);                                         // construct csview from cstr
csview        csview_from(const char* str);                         // construct from (const char*)
csview        csview_from_n(const char* str, size_t n);             // construct 
csview        csview_from_s(cstr s);                                // same as c_sv()
csview        csview_lit(const char literal_only[]);                // same as c_lit()

size_t        csview_size(csview sv);
size_t        csview_length(csview sv);
bool          csview_empty(csview sv);
void          csview_clear(csview* self);

csview        csview_substr(csview sv, size_t pos, size_t n);
csview        csview_trimmed(csview sv, size_t left, size_t right);
csview        csview_first_token(csview sv, csview sep);
csview        csview_next_token(csview sv, csview sep, csview token);

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
csview        cstr_trimmed(cstr s, size_t left, size_t right);
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

| Name             | Value               | Usage                             |
|:-----------------|:--------------------|:----------------------------------|
| `csview_null`    | same as `c_lit("")` | `sview = csview_null;`            |
| `c_lit(literal)` | csview constructor  | `sview = c_lit("hello, world");`  |
| `csview_arg(sv)` | printf argument     | `printf("%.*s", csview_arg(sv));` |

## cstr-containers with csview emplace/lookup API
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

int main()
{
    csview text = c_lit("The length of this literal is evaluated at compile time and stored in csview text.");
    printf("%s\nLength: %zu\n\n", text.str, text.size);

    c_forvar (cmap_si map = csmap_si_init(), csmap_si_del(&map))
    {
        cmap_si_emplace(&map, c_lit("hello"), 100);
        cmap_si_emplace(&map, c_lit("world"), 200);
        cmap_si_emplace(&map, c_lit("hello"), 300); // already in map, ignored

        // Efficient lookup: no string allocation or strlen() takes place:
        cmap_si_value_t* v = cmap_si_get(&map, c_lit("world"));
        printf("\n%s: %d\n", v->first.str, v->second);
    }
}
```
Output:
```
A long and winded literal string
Length: 32

world: 200
```

### Example 2: csview tokenizer (string split)
Splits strings into tokens. *print_split()* calls make **no** memory allocations, *strlen()* calls, or depends on
null-terminated strings. *string_split()* function returns a vector of cstr.
```c
#include <stc/csview.h>
#include <stc/cvec.h>

void print_split(csview str, csview sep)
{
    csview token = csview_first_token(str, sep);
    for (;;) {
        // print non-null-terminated csview
        printf("\"%.*s\"\n", csview_arg(token));
        if (csview_end(&token).ref == csview_end(&str).ref) break;
        token = csview_next_token(str, sep, token);
    }
}

using_cvec_str();

cvec_str string_split(csview str, csview sep)
{
    cvec_str vec = cvec_str_init();
    csview token = csview_first_token(str, sep);
    for (;;) {
        cvec_str_push_back(&vec, cstr_from_v(token));
        if (csview_end(&token).ref == csview_end(&str).ref) break;
        token = csview_next_token(str, sep, token);
    }
    return vec;
}

int main()
{
    print_split(c_lit("//This is a//double-slash//separated//string"), c_lit("//"));
    puts("");
    print_split(c_lit("This has no matching separator"), c_lit("xx"));
    puts("");

    c_forvar (cvec_str v = string_split(c_lit("Split,this,,string,now,"), c_lit(",")), cvec_str_del(&v))
        c_foreach (i, cvec_str, v)
            printf("\"%s\"\n", i.ref->str);
}
```
Output:
```
""
"This is a"
"double-slash"
"separated"
"string"

"This has no matching separator"

"Split"
"this"
""
"string"
"now"
""
```
