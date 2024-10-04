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
zsview          zsview_from_position(zsview zv, isize pos);     // subview starting from index pos
zsview          zsview_trailing(zsview zv, isize len);          // subview of the traling n bytes

isize           zsview_size(zsview zv);
bool            zsview_is_empty(zsview zv);                     // check if size == 0
void            zsview_clear(zsview* self);

csview          zsview_sv(zsview zv);                           // convert to csview type
bool            zsview_equals(zsview zv, const char* str);
bool            zsview_iequals(zsview zs, const char* str);
isize           zsview_find(zsview zv, const char* str);
bool            zsview_contains(zsview zv, const char* str);
bool            zsview_starts_with(zsview zv, const char* str);
bool            zsview_istarts_with(zsview zs, const char* str);
bool            zsview_ends_with(zsview zv, const char* str);
bool            zsview_iends_with(zsview zs, const char* str);

/* utf8 */
zsview_iter     zsview_begin(const zsview* self);
zsview_iter     zsview_end(const zsview* self);
void            zsview_next(zsview_iter* it);                   // next rune
zsview_iter     zsview_advance(zsview_iter it, isize u8pos);    // advance +/- runes

zsview          zsview_u8_from_position(zsview zv, isize i8pos);// subview starting from rune i8pos
zsview          zsview_u8_trailing(zsview zv, isize u8len);     // subview of the last u8len runes
csview          zsview_u8_chr(zsview zv, isize i8pos);          // get rune at rune position
isize           zsview_u8_count(zsview zv);                     // number of utf8 runes
bool            zsview_u8_valid(zsview zv);                     // check utf8 validity of zv

#### Helper methods for usage in containers
```c
uint64_t        zsview_hash(const zsview* x);
int             zsview_cmp(const zsview* x, const zsview* y);
bool            zsview_eq(const zsview* x, const zsview* y);
int             zsview_icmp(const zsview* x, const zsview* y);  // utf8 case-insensitive comparison
bool            zsview_ieq(const zsview* x, const zsview* y);   // utf8 case-insensitive comparison
```

#### UTF8 helper methods
```c
                // from utf8.h
isize           utf8_count(const char *s);                      // number of utf8 runes in s
isize           utf8_count_n(const char *s, isize nbytes);      // number of utf8 runes within n bytes
const char*     utf8_at(const char *s, isize pos);              // from utf8 pos to char* 
isize           utf8_to_index(const char* s, isize pos);        // from utf8 pos to byte index
unsigned        utf8_chr_size(const char* s);                   // utf8 character size: 1-4

                // requires linking with utf8 symbols
bool            utf8_valid(const char* s);
bool            utf8_valid_n(const char* s, isize nbytes);
uint32_t        utf8_decode(utf8_decode_t *d, uint8_t byte);    // decode next byte to utf8, return state.
unsigned        utf8_encode(char *out, uint32_t codepoint);     // encode unicode cp into out buffer
uint32_t        utf8_peek(const char* s);                       // codepoint value of character at s
uint32_t        utf8_peek_from(const char* s, isize offset);    // codepoint value at utf8 offset (may be negative)
int             utf8_icmp(const char* s1, const char* s2);      // case-insensitive comparison
int             utf8_icompare(csview s1, csview s2);            // case-insensitive comparison

uint32_t        utf8_casefold(uint32_t c);                      // fold to a non-unique lowercase char.
uint32_t        utf8_tolower(uint32_t c);
uint32_t        utf8_toupper(uint32_t c);

bool            utf8_isalpha(uint32_t c);
bool            utf8_isalnum(uint32_t c);
bool            utf8_isword(uint32_t c);
bool            utf8_iscased(uint32_t c);
bool            utf8_isblank(uint32_t c);
bool            utf8_isspace(uint32_t c);
bool            utf8_isupper(uint32_t c);
bool            utf8_islower(uint32_t c);
bool            utf8_isgroup(int group, uint32_t c);
```

## Types

| Type name      | Type definition                              | Used to represent...     |
|:---------------|:---------------------------------------------|:-------------------------|
| `zsview`       | `struct { const char *str; isize size; }` | The string view type   |
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
