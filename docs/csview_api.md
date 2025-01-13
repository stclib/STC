# STC [csview](../include/stc/csview.h): Non-zero terminated String View
![String](pics/string.jpg)

The type **csview** is a ***non-zero terminated*** and ***utf8-iterable*** string view. It refers to a
constant contiguous sequence of char-elements with the first element of the sequence at position zero.
The implementation holds two members: a pointer to constant char and a size. See [zsview](zsview_api.md)
for a ***zero-terminated*** string view/span type.

**csview** never allocates memory, and therefore need not be destructed. Its lifetime is limited by the
source string storage. It keeps the length of the string, which redcues the need to call *strlen()* in
usage.

- **csview** iterators works on UTF8 codepoints - like **cstr** and **zsview** (see Example 2).
- Because it is not zero-terminated, it must be printed the following way:
```c++
csview sv = c_sv("Hello world");
sv = csview_subview(sv, 0, 5);
printf(c_svfmt "\n", c_svarg(sv)); // "Hello"
```

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
for a functional description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c++
#include "stc/cstr.h"
#include "stc/csview.h" // after cstr.h: include extra cstr-csview functions
```
## Methods

```c++
csview          c_sv(const char literal_only[]);                        // from string literal only
csview          c_sv(const char* str, isize n);                         // from a const char* and length n
csview          csview_from(const char* str);                           // from const char* str
csview          csview_with_n(const char* str, isize n);                // alias for c_sv(str, n)

isize           csview_size(csview sv);
bool            csview_is_empty(csview sv);
void            csview_clear(csview* self);

bool            csview_equals(csview sv, const char* str);
isize           csview_find(csview sv, const char* str);
isize           csview_find_sv(csview sv, csview find);
bool            csview_contains(csview sv, const char* str);
bool            csview_starts_with(csview sv, const char* str);
bool            csview_ends_with(csview sv, const char* str);

csview          csview_subview(csview sv, isize pos, isize len);
csview          csview_slice(csview sv, isize pos1, isize pos2);
csview          csview_tail(csview sv, isize len);                      // span of the trailing len bytes
csview          csview_trim(csview sv);                                 // trim whitespace and ctrl-chars on both ends
csview          csview_trim_start(csview sv);                           // trim from start of view
csview          csview_trim_end(csview sv);                             // trim from end of view

csview          csview_subview_pro(csview sv, isize pos, isize len);    // negative pos count from end
csview          csview_token(csview sv, const char* sep, isize* start); // *start > sv.size after last token
```

#### UTF8 methods
```c++
csview          csview_u8_from(const char* str, isize u8pos, isize u8len); // construct csview with u8len runes
isize           csview_u8_size(csview sv);                              // number of utf8 runes
csview_iter     csview_u8_at(csview sv, isize u8pos);                   // get rune at rune position
csview          csview_u8_subview(csview sv, isize u8pos, isize u8len); // utf8 span
csview          csview_u8_tail(csview sv, isize u8len);                 // span of the trailing u8len runes.
bool            csview_u8_valid(csview sv);                             // check utf8 validity of sv

bool            csview_iequals(csview sv, const char* str);             // utf8 case-insensitive comparison
bool            csview_istarts_with(csview sv, const char* str);        // utf8 case-insensitive
bool            csview_iends_with(csview sv, const char* str);          // utf8 case-insensitive

csview_iter     csview_begin(const csview* self);                       // utf8 iteration
csview_iter     csview_end(const csview* self);
void            csview_next(csview_iter* it);                           // next utf8 codepoint
csview_iter     csview_advance(csview_iter it, isize u8pos);            // advance +/- codepoints
```

#### Iterate tokens with *c_token*

Iterate tokens in an input string split by a separator string:
- `for (c_token_sv(it, const char* separator, csview input_sv)) ...;`
- `it.token` is a csview of the current token.

```c++
for (c_token(i, ", ", "hello, one, two, three"))
    printf("'" c_svfmt "' ", c_svarg(i.token));
// 'hello' 'one' 'two' 'three'
```

#### Helper methods
```c++
size_t          csview_hash(const csview* x);
int             csview_cmp(const csview* x, const csview* y);
bool            csview_eq(const csview* x, const csview* y);
int             csview_icmp(const csview* x, const csview* y);
bool            csview_ieq(const csview* x, const csview* y);
```

## Types

| Type name       | Type definition                            | Used to represent...     |
|:----------------|:-------------------------------------------|:-------------------------|
| `csview`        | `struct { const char *buf; isize size; }` | The string view type   |
| `csview_value`  | `const char`                               | The string element type  |
| `csview_iter`   | `union { csview_value *ref; csview chr; }` | UTF8 iterator            |

## Constants and macros

| Name           | Value                | Usage                                        |
|:---------------|:---------------------|:---------------------------------------------|
| `c_svarg(sv)`  | printf argument      | `printf("sv: " c_svfmt "\n", c_svarg(sv));`         |

## Example
```c++
#include "stc/cstr.h"
#include "stc/csview.h"


int main(void)
{
    cstr str = cstr_lit("We think in generalities, but we live in details.");
    csview sv = cstr_sv(&str);
    csview sv1 = csview_subview(sv, 3, 5);                 // "think"
    isize pos = csview_find(sv, "live");                   // position of "live"
    csview sv2 = csview_subview(sv, pos, 4);               // "live"
    csview sv3 = csview_subview_pro(sv, -8, 7);            // "details"
    printf(c_svfmt ", " c_svfmt ", " c_svfmt "\n",
           c_svarg(sv1), c_svarg(sv2), c_svarg(sv3));

    cstr_assign(&str, "apples are green or red");
    sv = cstr_sv(&str);
    cstr s2 = cstr_from_sv(csview_subview_pro(sv, -3, 3)); // "red"
    cstr s3 = cstr_from_sv(csview_subview(sv, 0, 6));      // "apples"

    c_drop(cstr, &str, &s2, &s3);
}
```
Output:
```
think live details
red Apples
```

### Example 2: UTF8 handling
```c++
#include "stc/cstr.h"

int main(void)
{
    cstr s1 = cstr_lit("hell😀 w😀rld");

    cstr_u8_replace(&s1, 7, 1, "ø");
    printf("%s\n", cstr_str(&s1));

    for (c_each(i, cstr, s1))
        printf(c_svfmt ",", c_svarg(i.chr));

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
and does not depend on zero-terminated strings. *string_split()* function returns a vector of cstr.
```c++
#include <stdio.h>
#include "stc/csview.h"

void print_split(csview input, const char* sep)
{
    for (c_token_sv(i, sep, input))
        printf("[" c_svfmt "]\n", c_svarg(i.token));
    puts("");
}
#include "stc/cstr.h"
#define i_keypro cstr
#include "stc/stack.h"

stack_cstr string_split(csview input, const char* sep)
{
    stack_cstr out = {0};

    for (c_token(i, sep, input))
        stack_cstr_push(&out, cstr_from_sv(i.token));

    return out;
}

int main(void)
{
    print_split(c_sv("//This is a//double-slash//separated//string"), "//");
    print_split(c_sv("This has no matching separator"), "xx");

    stack_cstr s = string_split(c_sv("Split,this,,string,now,"), ",");

    for (c_each(i, stack_cstr, s))
        printf("[%s]\n", cstr_str(i.ref));
    puts("");

    stack_cstr_drop(&s);
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
