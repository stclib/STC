# STC [zsview](../include/stc/zsview.h): Zero-terminated UTF8 String View
![String](pics/string.jpg)

The type **zsview** is a ***zero-terminated*** and ***utf8-iterable*** string view. It refers to a
constant contiguous sequence of char-elements with the first element of the sequence at position zero.
The implementation holds two members: a pointer to constant char and a size. See [csview](csview_api.md)
for a ***non zero-terminated*** string view/span type.

Because **zsview** is zero-terminated, it can be an efficient replacent for `const char*`. E.g., the .str
char* pointer can safely be passed to c-api's which expects standard zero terminated c-strings. It never
allocates memory, and therefore need not be destructed. Its lifetime is limited by the source string
storage. It keeps the length of the string, i.e. no need to call *strlen()* for various operations.

## Header file

All zsview definitions and prototypes are available by including a single header file.

```c++
#include "stc/zsview.h"
```

## Methods
```c++
zsview          c_zv(const char literal_only[]);                    // create from string literal only
zsview          zsview_from(const char* str);                       // construct from const char*
zsview          zsview_from_pos(zsview zv, isize pos);              // subview starting from index pos

isize           zsview_size(zsview zv);
bool            zsview_is_empty(zsview zv);                         // check if size == 0
void            zsview_clear(zsview* self);

csview          zsview_sv(zsview zv);                               // convert to csview type
csview          zsview_subview(zsview zv, isize pos, isize len);    // return as csview span
zsview          zsview_tail(zsview zv, isize len);                  // subview of the trailing n bytes

bool            zsview_equals(zsview zv, const char* str);
isize           zsview_find(zsview zv, const char* str);
bool            zsview_contains(zsview zv, const char* str);
bool            zsview_starts_with(zsview zv, const char* str);
bool            zsview_ends_with(zsview zv, const char* str);
```

#### UTF8 methods
```c++
zsview          zsview_u8_from_pos(zsview zv, isize u8pos);         // subview starting from rune u8pos
isize           zsview_u8_size(zsview zv);                          // number of utf8 runes
zsview_iter     zsview_u8_at(zsview zv, isize u8pos);               // get rune at rune position
csview          zsview_u8_subview(zsview zs, isize u8pos, isize u8len);
zsview          zsview_u8_tail(zsview zv, isize u8len);             // subview of the last u8len runes
bool            zsview_u8_valid(zsview zv);                         // check utf8 validity of zv

bool            zsview_iequals(zsview zs, const char* str);         // utf8 case-insensitive comparison
bool            zsview_istarts_with(zsview zs, const char* str);    // utf8 case-insensitive
bool            zsview_iends_with(zsview zs, const char* str);      // utf8 case-insensitive

zsview_iter     zsview_begin(const zsview* self);                   // utf8 iteration
zsview_iter     zsview_end(const zsview* self);
void            zsview_next(zsview_iter* it);                       // next rune
zsview_iter     zsview_advance(zsview_iter it, isize u8pos);        // advance +/- runes
```

#### Helper methods for usage in containers
```c++
size_t          zsview_hash(const zsview* x);
int             zsview_cmp(const zsview* x, const zsview* y);
bool            zsview_eq(const zsview* x, const zsview* y);
int             zsview_icmp(const zsview* s1, const zsview* s2);    // utf8 case-insensitive comparison
bool            zsview_ieq(const zsview* s1, const zsview* s2);     // "
```

## Types

| Type name      | Type definition                              | Used to represent...     |
|:---------------|:---------------------------------------------|:-------------------------|
| `zsview`       | `struct { const char *str; isize size; }` | The string view type   |
| `zsview_value` | `const char`                                 | The element type         |
| `zsview_iter`  | `union { zsview_value *ref; csview chr; }`   | UTF8 iterator           |

## Example: UTF8 iteration and case conversion
```c++
#include "stc/cstr.h"
#include "stc/zsview.h"

int main(void)
{
    zsview say = c_zv("Liberté, égalité, fraternité.");
    printf("%s\n", zs.str);

    for (c_each(i, zsview, say))
        printf(c_svfmt " ", c_svarg(i.chr));
    puts("");

    cstr upper = cstr_toupper_sv(zsview_sv(say));
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
