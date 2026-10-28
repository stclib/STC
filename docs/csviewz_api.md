# STC [csviewz](../include/stc/csviewz.h): Zero-terminated UTF8 String View
![String](pics/string.jpg)

The type **csviewz** is a ***zero-terminated*** and ***UTF-8 iterable*** string view. It refers to a
constant contiguous sequence of char-elements with the first element of the sequence at position zero.
The implementation holds two members: a pointer to constant char and a size. See [csview](csview_api.md)
for a ***non zero-terminated*** string view/span type.

Because **csviewz** is zero-terminated, it can be an efficient replacement for `const char*`. E.g., the .str
char* pointer can safely be passed to C API's which expects standard zero terminated c-strings. It never
allocates memory, and therefore need not be destructed. Its lifetime is limited by the source string
storage. It keeps the length of the string, i.e. no need to call *strlen()* for various operations.

## Header file

All **csviewz** definitions and prototypes are available by including a single header file.

```c++
#include <stc/csviewz.h>
```

## Methods
```c++
csviewz         c_zv(const char literal_only[]);                      // create from string literal only
csviewz         csviewz_from(const char* str);                        // construct from const char*
csviewz         csviewz_from_pos(csviewz zv, isize_t pos);            // subview starting from index pos

isize_t         csviewz_size(csviewz zv);
bool            csviewz_is_empty(csviewz zv);                         // check if size == 0
void            csviewz_clear(csviewz* self);

csview          csviewz_sv(csviewz zv);                               // convert to csview type
csview          csviewz_subview(csviewz zv, isize_t pos, isize_t len);// return as csview span
csviewz         csviewz_tail(csviewz zv, isize_t len);                // subview of the trailing n bytes

bool            csviewz_equals(csviewz zv, const char* str);
isize_t         csviewz_find(csviewz zv, const char* str);
bool            csviewz_contains(csviewz zv, const char* str);
bool            csviewz_starts_with(csviewz zv, const char* str);
bool            csviewz_ends_with(csviewz zv, const char* str);
```

#### UTF8 methods
```c++
csviewz         csviewz_u8_from_pos(csviewz zv, isize_t u8pos);       // subview starting from rune u8pos
isize_t         csviewz_u8_size(csviewz zv);                          // number of utf8 runes
csviewz_iter    csviewz_u8_at(csviewz zv, isize_t u8pos);             // get rune at rune position
csview          csviewz_u8_subview(csviewz zs, isize_t u8pos, isize_t u8len);
csviewz         csviewz_u8_tail(csviewz zv, isize_t u8len);           // subview of the last u8len runes
bool            csviewz_u8_valid(csviewz zv);                         // check utf8 validity of zv

bool            csviewz_iequals(csviewz zs, const char* str);         // utf8 case-insensitive comparison
bool            csviewz_istarts_with(csviewz zs, const char* str);    // utf8 case-insensitive
bool            csviewz_iends_with(csviewz zs, const char* str);      // utf8 case-insensitive

csviewz_iter    csviewz_begin(const csviewz* self);                   // utf8 iteration
csviewz_iter    csviewz_end(const csviewz* self);
void            csviewz_next(csviewz_iter* it);                       // next rune
csviewz_iter    csviewz_advance(csviewz_iter it, isize_t u8pos);      // advance +/- runes
uint32_t        csviewz_codepoint(const csviewz_iter* it);            // return cached codepoint for iter
```

#### Helper methods for usage in containers
```c++
size_t          csviewz_hash(const csviewz* x);
int             csviewz_cmp(const csviewz* x, const csviewz* y);
bool            csviewz_eq(const csviewz* x, const csviewz* y);
int             csviewz_icmp(const csviewz* s1, const csviewz* s2);    // utf8 case-insensitive comparison
bool            csviewz_ieq(const csviewz* s1, const csviewz* s2);     // "
```

## Types

| Type name      | Type definition                              | Used to represent...     |
|:---------------|:---------------------------------------------|:-------------------------|
| `csviewz`       | `struct { const char *str; isize_t size; }` | The string view type   |
| `csviewz_value` | `const char`                                | The element type         |
| `csviewz_iter`  | `union { csviewz_value *ref; csview chr; }` | UTF8 iterator           |

## Example: UTF8 iteration and case conversion
```c++
#include <stc/cstr.h>
#include <stc/csviewz.h>

int main(void)
{
    csviewz say = c_zv("Liberté, égalité, fraternité.");
    printf("%s\n", zs.str);

    for (c_each(i, csviewz, say))
        printf(c_svfmt " ", c_svarg(i.chr));
    puts("");

    cstr upper = cstr_toupper_sv(csviewz_sv(say));
    printf("%s\n", cstr_str(&upper));
    cstr_drop(&upper);
}
```
Output:
```
Liberté, égalité, fraternité.
L i b e r t é ,   é g a l i t é ,   f r a t e r n i t é .
LIBERTÉ, ÉGALITÉ, FRATERNITÉ.
```
