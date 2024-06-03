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

```c
#include "stc/zsview.h"
```
## Methods

```c
zsview          c_zv(const char literal_only[]);                // create from string literal only
zsview          zsview_from(const char* str);                   // construct from const char*
zsview          zsview_from_pos(zsview zv, intptr_t pos);       // subview starting from pos to eos.
zsview          zsview_last(zsview zv, intptr_t count);         // subview of the last count bytes

intptr_t        zsview_size(zsview zv);
bool            zsview_is_empty(zsview zv);                     // check if size == 0
void            zsview_clear(zsview* self);

csview          zsview_sv(zsview zv);                           // convert to csview type
bool            zsview_equals(zsview zv, const char* str);
intptr_t        zsview_find(zsview zv, const char* str);
bool            zsview_contains(zsview zv, const char* str);
bool            zsview_starts_with(zsview zv, const char* str);
bool            zsview_ends_with(zsview zv, const char* str);

zsview_iter     zsview_begin(const zsview* self);
zsview_iter     zsview_end(const zsview* self);
void            zsview_next(zsview_iter* it);                   // next utf8 codepoint
zsview_iter     zsview_advance(zsview_iter it, intptr_t u8pos); // advance +/- codepoints

/* utf8 */
const char*     zsview_u8_at(zsview zv, intptr_t u8idx);
zsview          zsview_u8_from_pos(zsview zv, intptr_t u8idx);  // subview starting from u8idx
zsview          zsview_u8_last(zsview zv, intptr_t u8len);      // subview of the last u8len codepoints
intptr_t        zsview_u8_size(zsview zv);
bool            zsview_u8_valid(zsview zv);                     // requires linking with utf8 symbols
```

#### Helper methods for usage in containers
```c
int             zsview_cmp(const zsview* x, const zsview* y);
int             zsview_icmp(const zsview* x, const zsview* y);  // requires linking with utf8 symbols
bool            zsview_eq(const zsview* x, const zsview* y);
uint64_t        zsview_hash(const zsview* x);
```

#### UTF8 helper methods
```c
                // from utf8.h
intptr_t        utf8_size(const char *s);
intptr_t        utf8_size_n(const char *s, intptr_t nbytes);        // number of UTF8 codepoints within n bytes
const char*     utf8_at(const char *s, intptr_t index);             // from UTF8 index to char* position
intptr_t        utf8_pos(const char* s, intptr_t index);            // from UTF8 index to byte index position
unsigned        utf8_chr_size(const char* s);                       // UTF8 character size: 1-4

                // requires linking with utf8 symbols
bool            utf8_valid(const char* s);
bool            utf8_valid_n(const char* s, intptr_t nbytes);
uint32_t        utf8_decode(utf8_decode_t *d, uint8_t byte);        // decode next byte to utf8, return state.
unsigned        utf8_encode(char *out, uint32_t codepoint);         // encode unicode cp into out buffer
uint32_t        utf8_peek(const char* s);                           // codepoint value of character at s
uint32_t        utf8_peek_off(const char* s, int offset);           // codepoint value at utf8 pos (may be negative)
```

## Types

| Type name      | Type definition                              | Used to represent...     |
|:---------------|:---------------------------------------------|:-------------------------|
| `zsview`       | `struct { const char *str; intptr_t size; }` | The string view type   |
| `zsview_value` | `const char`                                 | The element type         |
| `zsview_iter`  | `union { zsview_value *ref; csview chr; }`   | UTF8 iterator           |

## Example: UTF8 iteration and case conversion
```c
#define i_import
#include "stc/cstr.h"
#include "stc/zsview.h"

int main(void)
{
    zsview say = c_zv("Liberté, égalité, fraternité.");
    printf("%s\n", zs.str);

    c_foreach (i, zsview, say)
        printf("%.*s ", c_SVARG(i.chr));
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
