# STC [csview](../include/stc/csview.h): String View
![String](pics/string.jpg)

The type **csview** is a string view and can refer to a constant contiguous sequence of char-elements with the first
element of the sequence at position zero. The implementation holds two members: a pointer to constant char and a size.

**csview** is an efficient replacent for `const char*`. It never allocates memory, and therefore need not be destructed.
Its lifetime is limited by the source string storage. It keeps the length of the string, and does not call *strlen()*
when passing it around. It is faster when using`csview` as convertion type (raw) than `const char*` in associative
containers with cstr keys.

Note: a **csview** may ***not be null-terminated***, and must therefore be printed like: 
`printf("%.*s", csview_ARG(sv))`.

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) for a functional
description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#include <stc/cstr.h> // optional, include cstr+csview functionality
#include <stc/csview.h>
```
## Methods

```c
csview          c_sv(const char literal_only[]);                        // construct from literal, no strlen()
csview          c_sv(const char* str, intptr_t n);                      // construct from str and length n
csview          csview_lit(const char literal_only[]);                  // alias for c_sv(lit)
csview          csview_from(const char* str);                           // construct from const char*
csview          csview_from_n(const char* str, intptr_t n);             // alias for c_sv(str, n)

intptr_t        csview_size(csview sv);
bool            csview_empty(csview sv);
void            csview_clear(csview* self);

bool            csview_equals(csview sv, csview sv2);
intptr_t        csview_find(csview sv, const char* str);
intptr_t        csview_find_sv(csview sv, csview find);
bool            csview_contains(csview sv, const char* str);
bool            csview_starts_with(csview sv, const char* str);
bool            csview_ends_with(csview sv, const char* str);

csview          csview_substr_ex(csview sv, intptr_t pos, intptr_t n);  // negative pos count from end
csview          csview_slice_ex(csview sv, intptr_t p1, intptr_t p2);   // negative p1, p2 count from end
csview          csview_token(csview sv, const char* sep, intptr_t* start); // *start > sv.size after last token
```

#### UTF8 methods
```c
intptr_t        csview_u8_size(csview sv);
csview          csview_u8_substr(csview sv, intptr_t bytepos, intptr_t u8len);
bool            csview_valid_utf8(csview sv);                           // requires linking with src/utf8code.c

csview_iter     csview_begin(const csview* self);
csview_iter     csview_end(const csview* self);
void            csview_next(csview_iter* it);                           // utf8 codepoint step, not byte!
csview_iter     csview_advance(csview_iter it, intptr_t n);

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

#### Extended cstr methods
```c
csview          cstr_substr(const cstr* self, intptr_t pos, intptr_t n);
csview          cstr_substr_ex(const cstr* s, intptr_t pos, intptr_t n); // negative pos count from end
csview          cstr_u8_substr(const cstr* self, intptr_t bytepos, intptr_t u8len);

csview          cstr_slice(const cstr* self, intptr_t p1, intptr_t p2);
csview          cstr_slice_ex(const cstr* s, intptr_t p, intptr_t q);    // negative p or q count from end
```
#### Iterate tokens with *c_fortoken*, *c_fortoken_sv*

To iterate tokens in an input string separated by a string:
```c
c_fortoken (i, "hello, one, two, three", ", ")
    printf("token: %.*s\n", c_SV(i.token));
```

#### Helper methods
```c
int             csview_cmp(const csview* x, const csview* y);
int             csview_icmp(const csview* x, const csview* y);
bool            csview_eq(const csview* x, const csview* y);
uint64_t        csview_hash(const csview* x);
```

## Types

| Type name       | Type definition                            | Used to represent...     |
|:----------------|:-------------------------------------------|:-------------------------|
| `csview`        | `struct { const char *str; intptr_t size; }` | The string view type     |
| `csview_value`  | `char`                                     | The string element type  |
| `csview_iter`   | `struct { csview_value *ref; }`            | UTF8 iterator            |

## Constants and macros

| Name           | Value                | Usage                                        |
|:---------------|:---------------------|:---------------------------------------------|
| `csview_NULL`  | same as `c_sv("")`   | `sview = csview_NULL;`                       |
| `c_SV(sv)`     | printf argument      | `printf("sv: %.*s\n", c_SV(sv));`            |

## Example
```c
#include <stc/cstr.h>
#include <stc/csview.h>

int main ()
{
    cstr str1 = cstr_lit("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csview sv1 = cstr_substr(&str1, 3, 5);              // "think"
    intptr_t pos = cstr_find(&str1, "live");              // position of "live" in str1
    csview sv2 = cstr_substr(&str1, pos, 4);            // get "live"
    csview sv3 = cstr_slice(&str1, -8, -1);             // get "details"
    printf("%.*s %.*s %.*s\n",
        c_SV(sv1), c_SV(sv2), c_SV(sv3));
    cstr s1 = cstr_lit("Apples are red");
    cstr s2 = cstr_from_sv(cstr_substr(&s1, -3, 3));    // "red"
    cstr s3 = cstr_from_sv(cstr_substr(&s1, 0, 6));     // "Apples"
    printf("%s %s\n", cstr_str(&s2), cstr_str(&s3));

    c_drop(cstr, &str1, &s1, &s2, &s3);
}
```
Output:
```
think live details
red Apples
```

### Example 2: UTF8 handling
```c
#include <stc/cstr.h>
#include <stc/csview.h>

int main()
{
    cstr s1 = cstr_lit("hell😀 w😀rld");

    cstr_u8_replace_at(&s1, cstr_find(&s1, "😀rld"), 1, c_sv("ø"));
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
        printf("%.*s,", c_SV(i.u8.chr));

    cstr_drop(&s1);
}
```
Output:
```
hell😀 wørld
h,e,l,l,😀, ,w,ø,r,l,d,
```

### Example 3: csview tokenizer (string split)
Splits strings into tokens. *print_split()* makes **no** memory allocations or *strlen()* calls,
and does not depend on null-terminated strings. *string_split()* function returns a vector of cstr.
```c
#include <stdio.h>
#include <stc/csview.h>

void print_split(csview input, const char* sep)
{
    c_fortoken_sv (i, input, sep)
        printf("[%.*s]\n", c_SV(i.token));
    puts("");
}

#include <stc/cstr.h>
#define i_val_str
#include <stc/cstack.h>

cstack_str string_split(csview input, const char* sep)
{
    cstack_str out = cstack_str_init();
    
    c_fortoken_sv (i, input, sep)
        cstack_str_push(&out, cstr_from_sv(i.token));

    return out;
}

int main()
{
    print_split(c_sv("//This is a//double-slash//separated//string"), "//");
    print_split(c_sv("This has no matching separator"), "xx");

    cstack_str s = string_split(c_sv("Split,this,,string,now,"), ",");

    c_foreach (i, cstack_str, s)
        printf("[%s]\n", cstr_str(i.ref));
    puts("");

    cstack_str_drop(&s);
}
```
Output:
```
[]
[This is a]
[double-slash]
[separated]
[string]

[This has no matching separator]

[Split]
[this]
[]
[string]
[now]
[]
```
