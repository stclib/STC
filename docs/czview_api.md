# STC [czview](../include/stc/czview.h): Null-terminated UTF8 String View
![String](pics/string.jpg)

The type **czview** is a ***null-terminated*** string view and refers to a constant contiguous sequence of
char-elements with the first element of the sequence at position zero. The implementation holds two
members: a pointer to constant char and a size. See [csview](csview_api.md) for a ***non null-terminated***
string view/span type.

Because **czview** is null-terminated, it can be an efficient replacent for `const char*`. It never
allocates memory, and therefore need not be destructed. Its lifetime is limited by the source string
storage. It keeps the length of the string, i.e. no need to call *strlen()* for various operations.

## Header file

All czview definitions and prototypes are available by including a single header file.

```c
#include "stc/czview.h"
```
## Methods

```c
czview          c_zv(const char literal_only[]);                // from literal str, no strlen()
czview          czview_from(const char* str);                   // construct from const char*
czview          czview_from_pos(czview zv, intptr_t pos);       // subview starting from pos
czview          czview_last(czview zv, intptr_t count);         // subview of the last count bytes

intptr_t        czview_size(czview zv);
bool            czview_empty(czview zv);                        // check if size == 0
void            czview_clear(czview* self);

csview          czview_sv(czview zv);                           // convert to csview type
bool            czview_equals(czview zv, const char* str);
intptr_t        czview_find(czview zv, const char* str);
bool            czview_contains(czview zv, const char* str);
bool            czview_starts_with(czview zv, const char* str);
bool            czview_ends_with(czview zv, const char* str);

czview_iter     czview_begin(const czview* self);
czview_iter     czview_end(const czview* self);
void            czview_next(czview_iter* it);                   // next utf8 codepoint
czview_iter     czview_advance(czview_iter it, intptr_t u8pos); // advance +/- codepoints

/* utf8 */
const char*     czview_u8_at(czview zv, intptr_t u8idx);
czview          czview_u8_from_pos(czview zv, intptr_t u8idx);  // subview starting from u8idx
czview          czview_u8_last(czview zv, intptr_t u8len);      // subview of the last u8len codepoints
intptr_t        czview_u8_size(czview zv);
bool            czview_u8_valid(czview zv);                     // requires linking with utf8 symbols
```

#### Helper methods for usage in containers
```c
int             czview_cmp(const czview* x, const czview* y);
int             czview_icmp(const czview* x, const czview* y);  // requires linking with utf8 symbols
bool            czview_eq(const czview* x, const czview* y);
uint64_t        czview_hash(const czview* x);
```

#### UTF8 methods
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
| `czview`       | `struct { const char *str; intptr_t size; }` | The string view type   |
| `czview_value` | `const char`                                 | The element type         |
| `czview_iter`  | `union { czview_value *ref; csview chr; }`   | UTF8 iterator           |

## Example: UTF8 iteration and case conversion
```c
#define i_import
#include "stc/cstr.h"
#include "stc/czview.h"

int main(void)
{
    czview say = c_zv("Liberté, égalité, fraternité.");
    printf("%s\n", zs.str);

    c_foreach (i, czview, say)
        printf("%.*s ", c_SV(i.chr));
    puts("");

    cstr upper = cstr_toupper_sv(czview_sv(say));
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
