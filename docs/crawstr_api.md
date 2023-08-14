# STC [crawstr](../include/stc/crawstr.h): Null-terminated UTF8 String View
![String](pics/string.jpg)

The type **crawstr** is a ***null-terminated*** string view and refers to a constant contiguous sequence of
char-elements with the first element of the sequence at position zero. The implementation holds two
members: a pointer to constant char and a size. See [csview](csview_api.md) for a ***non null-terminated***
string view/span type.

Because **crawstr** is null-terminated, it can be an efficient replacent for `const char*`. It never
allocates memory, and therefore need not be destructed. Its lifetime is limited by the source string
storage. It keeps the length of the string, i.e. no need to call *strlen()* for various operations.

## Header file

All crawstr definitions and prototypes are available by including a single header file.

```c
#define i_implement
#include <stc/cstr.h>
#include <stc/crawstr.h>
```
## Methods

```c
crawstr         crawstr_from(const char* str);                          // construct from const char*
crawstr         c_rs(const char literal_only[]);                        // construct from literal, no strlen()

intptr_t        crawstr_size(crawstr rs);
bool            crawstr_empty(crawstr rs);                              // check if size == 0
void            crawstr_clear(crawstr* self);
csview          crawstr_sv(crawstr rs);                                 // convert to csview type

bool            crawstr_equals(crawstr rs, const char* str);
intptr_t        crawstr_find(crawstr rs, const char* str);
bool            crawstr_contains(crawstr rs, const char* str);
bool            crawstr_starts_with(crawstr rs, const char* str);
bool            crawstr_ends_with(crawstr rs, const char* str);
```

#### UTF8 methods
```c
intptr_t        crawstr_u8_size(crawstr rs);
bool            crawstr_valid_utf8(crawstr rs);                         // depends on src/utf8code.c

crawstr_iter    crawstr_begin(const crawstr* self);
crawstr_iter    crawstr_end(const crawstr* self);
void            crawstr_next(crawstr_iter* it);                         // utf8 codepoint step, not byte!
crawstr_iter    crawstr_advance(crawstr_iter it, intptr_t n);

                // from utf8.h
intptr_t        utf8_size(const char *s);
intptr_t        utf8_size_n(const char *s, intptr_t nbytes);            // number of UTF8 codepoints within n bytes
const char*     utf8_at(const char *s, intptr_t index);                 // from UTF8 index to char* position
intptr_t        utf8_pos(const char* s, intptr_t index);                // from UTF8 index to byte index position
unsigned        utf8_chr_size(const char* s);                           // UTF8 character size: 1-4
                // implemented in src/utf8code.c:
bool            utf8_valid(const char* s);
bool            utf8_valid_n(const char* s, intptr_t nbytes);
uint32_t        utf8_decode(utf8_decode_t *d, uint8_t byte);            // decode next byte to utf8, return state.
unsigned        utf8_encode(char *out, uint32_t codepoint);             // encode unicode cp into out buffer
uint32_t        utf8_peek(const char* s);                               // codepoint value of character at s
uint32_t        utf8_peek_off(const char* s, int offset);               // codepoint value at utf8 pos (may be negative)
```

#### Helper methods
```c
int             crawstr_cmp(const crawstr* x, const crawstr* y);
int             crawstr_icmp(const crawstr* x, const crawstr* y);       // depends on src/utf8code.c:
bool            crawstr_eq(const crawstr* x, const crawstr* y);
uint64_t        crawstr_hash(const crawstr* x);
```

## Types

| Type name       | Type definition                            | Used to represent...     |
|:----------------|:-------------------------------------------|:-------------------------|
| `crawstr`       | `struct { const char *str; intptr_t size; }` | The string view type     |
| `crawstr_value` | `char`                                     | The string element type  |
| `crawstr_iter`  | `struct { crawstr_value *ref; }`           | UTF8 iterator            |

## Example: UTF8 iteration and case conversion
```c
#define i_import
#include <stc/cstr.h>
#include <stc/crawstr.h>

int main(void)
{
    cstr str = cstr_from("Liberté, égalité, fraternité.");
    crawstr rs = cstr_rs(&str);

    c_foreach (i, crawstr, rs)
        printf("%.*s ", c_SV(i.u8.chr));
    puts("");

    cstr_uppercase(&str);
    printf("%s\n", cstr_str(&str));

    cstr_drop(&str);
}
```
Output:
```
L i b e r t é ,   é g a l i t é ,   f r a t e r n i t é . 
LIBERTÉ, ÉGALITÉ, FRATERNITÉ.
```

### Example 2: UTF8 replace
```c
#define i_import // include dependent utf8 definitions.
#include <stc/cstr.h>

int main(void)
{
    cstr s1 = cstr_lit("hell😀 w😀rld");

    cstr_u8_replace_at(&s1, cstr_find(&s1, "😀rld"), 1, c_rs("ø"));
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
        printf("%.*s,", c_SV(i.u8.chr)); // u8.chr is a csview

    cstr_drop(&s1);
}
```
Output:
```
hell😀 wørld
h,e,l,l,😀, ,w,ø,r,l,d,
```
