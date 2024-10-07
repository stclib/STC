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
```c
csview sv = c_sv("Hello world");
sv = csview_subview(sv, 0, 5);
printf(c_svfmt "\n", c_svarg(sv)); // "Hello"
```

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
for a functional description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#include "stc/cstr.h"
#include "stc/csview.h" // after cstr.h: include extra cstr-csview functions
```
## Methods

```c
csview          c_sv(const char literal_only[]);                        // from string literal only
csview          c_sv(const char* str, isize n);                         // from a const char* and length n
csview          csview_from(const char* str);                           // from const char* str
csview          csview_with_n(const char* str, isize n);                // alias for c_sv(str, n)

isize           csview_size(csview sv);
bool            csview_is_empty(csview sv);
void            csview_clear(csview* self);

bool            csview_equals(csview sv, const char* str);
bool            csview_iequals(csview sv, const char* str);
isize           csview_find(csview sv, const char* str);
isize           csview_find_sv(csview sv, csview find);
bool            csview_contains(csview sv, const char* str);
bool            csview_starts_with(csview sv, const char* str);
bool            csview_istarts_with(csview sv, const char* str);
bool            csview_ends_with(csview sv, const char* str);
bool            csview_iends_with(csview sv, const char* str);

csview          csview_subview(csview sv, isize pos, isize n);
csview          csview_slice(csview sv, isize pos1, isize pos2);
csview          csview_trim(csview sv);                                 // trim whitespaces from left+right of view
csview          csview_trim_left(csview sv);
csview          csview_trim_right(csview sv);
csview          csview_right(csview sv, isize len);                     // substr of the trailing len bytes
const char*     csview_at(csview sv, isize index);

csview          csview_subview_ex(csview sv, isize pos, isize n);       // negative pos count from end
csview          csview_slice_ex(csview sv, isize pos1, isize pos2);     // negative pos1, pos2 count from end
csview          csview_token(csview sv, const char* sep, isize* start); // *start > sv.size after last token
```

#### UTF8 methods
```c
csview         csview_u8_from(const char* str, isize u8pos, isize u8len); // construct csview with u8len runes
csview         csview_u8_subview(csview sv, isize u8pos, isize u8len);  // utf8 subview
csview         csview_u8_right(csview sv, isize u8len);                 // substr of the trailing u8len runes.
csview         csview_u8_chr(csview sv, isize u8pos);                   // get rune at rune position
isize          csview_u8_size(csview sv);                               // number of utf8 runes
bool           csview_u8_valid(csview sv);                              // check utf8 validity of sv

csview_iter    csview_begin(const csview* self);
csview_iter    csview_end(const csview* self);
void           csview_next(csview_iter* it);                            // next utf8 codepoint
csview_iter    csview_advance(csview_iter it, isize u8pos);             // advance +/- codepoints
```

#### Iterate tokens with *c_fortoken*

Iterate tokens in an input string split by a separator string:
- `c_fortoken_sv (it, const char* separator, csview input_sv);`
- `it.token` is a csview of the current token.

```c
c_fortoken (i, ", ", "hello, one, two, three")
    printf("'" c_svfmt "' ", c_svarg(i.token));
// 'hello' 'one' 'two' 'three'
```

#### Helper methods
```c
int            csview_cmp(const csview* x, const csview* y);
int            csview_icmp(const csview* x, const csview* y);
bool           csview_eq(const csview* x, const csview* y);
bool           csview_ieq(const csview* x, const csview* y);
size_t         csview_hash(const csview* x);
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
```c
#include "stc/cstr.h"
#include "stc/csview.h"

int main(void)
{
    cstr str1 = cstr_lit("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csview ss1 = cstr_subview_ex(&str1, 3, 5);           // "think"
    isize pos = cstr_find(&str1, "live");            // position of "live" in str1
    csview ss2 = cstr_subview_ex(&str1, pos, 4);         // get "live"
    csview ss3 = cstr_slice_ex(&str1, -8, -1);          // get "details"
    printf(c_svfmt " " c_svfmt " " c_svfmt "\n",
           c_svarg(ss1), c_svarg(ss2), c_svarg(ss3));
    cstr s1 = cstr_lit("Apples are red");
    cstr s2 = cstr_from_sv(cstr_subview_ex(&s1, -3, 3)); // "red"
    cstr s3 = cstr_from_sv(cstr_subview_ex(&s1, 0, 6));  // "Apples"
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
#define i_import // include dependent cstr, utf8 and cregex function definitions.
#include "stc/cstr.h"

int main(void)
{
    cstr s1 = cstr_lit("hell😀 w😀rld");

    cstr_u8_replace(&s1, 7, 1, "ø");
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
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
```c
#include <stdio.h>
#include "stc/csview.h"

void print_split(csview input, const char* sep)
{
    c_fortoken_sv (i, sep, input)
        printf("[" c_svfmt "]\n", c_svarg(i.token));
    puts("");
}
#include "stc/cstr.h"
#define i_key_cstr
#include "stc/stack.h"

stack_cstr string_split(csview input, const char* sep)
{
    stack_cstr out = {0};

    c_fortoken (i, sep, input)
        stack_cstr_push(&out, cstr_from_sv(i.token));

    return out;
}

int main(void)
{
    print_split(c_sv("//This is a//double-slash//separated//string"), "//");
    print_split(c_sv("This has no matching separator"), "xx");

    stack_cstr s = string_split(c_sv("Split,this,,string,now,"), ",");

    c_foreach (i, stack_cstr, s)
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
