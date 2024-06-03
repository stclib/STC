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
sv = csview_substr(sv, 0, 5);
printf("%.*s\n", sv.size, sv.buf);
// or with the c_SVARG() macro:
printf("%.*s\n", c_SVARG(sv));
```

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
for a functional description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#define i_implement
#include "stc/cstr.h"
#include "stc/csview.h" // after cstr.h: include extra cstr-csview functions
```
## Methods

```c
csview         c_sv(const char literal_only[]);                     // from string literal only
csview         c_sv(const char* str, intptr_t n);                   // from a const char* and length n
csview         csview_from(const char* str);                        // from const char* str
csview         csview_from_n(const char* str, intptr_t n);          // alias for c_sv(str, n)

intptr_t       csview_size(csview sv);
bool           csview_is_empty(csview sv);
void           csview_clear(csview* self);

bool           csview_equals(csview sv, const char* str);
intptr_t       csview_equals_sv(csview sv, csview sv2);
intptr_t       csview_find(csview sv, const char* str);
intptr_t       csview_find_sv(csview sv, csview find);
bool           csview_contains(csview sv, const char* str);
bool           csview_starts_with(csview sv, const char* str);
bool           csview_ends_with(csview sv, const char* str);

csview         csview_substr(csview sv, intptr_t pos, intptr_t n);
csview         csview_slice(csview sv, intptr_t pos1, intptr_t pos2);
csview         csview_last(csview sv, intptr_t len);                      // substr of the last len bytes
const char*    csview_at(csview sv, intptr_t index);

csview         csview_substr_ex(csview sv, intptr_t pos, intptr_t n);     // negative pos count from end
csview         csview_slice_ex(csview sv, intptr_t pos1, intptr_t pos2);  // negative pos1, pos2 count from end
csview         csview_token(csview sv, const char* sep, intptr_t* start); // *start > sv.size after last token
```

#### UTF8 methods
```c
intptr_t       csview_u8_size(csview sv);
csview         csview_u8_substr(csview sv, intptr_t bytepos, intptr_t u8len);
csview         csview_u8_last(csview sv, intptr_t u8len);                 // substr of the last u8len
const char*    csview_u8_at(csview sv, intptr_t u8idx);
bool           csview_u8_valid(csview sv);                                // requires linking with utf8 symbols

csview_iter    csview_begin(const csview* self);
csview_iter    csview_end(const csview* self);
void           csview_next(csview_iter* it);                              // next utf8 codepoint
csview_iter    csview_advance(csview_iter it, intptr_t u8pos);            // advance +/- codepoints
```

#### cstr methods returning csview
```c
csview         cstr_slice(const cstr* self, intptr_t pos1, intptr_t pos2);
csview         cstr_slice_ex(const cstr* self, intptr_t pos1, intptr_t pos2); // see csview_slice_ex()
csview         cstr_substr(const cstr* self, intptr_t pos, intptr_t n);
csview         cstr_substr_ex(const cstr* self, intptr_t pos, intptr_t n);    // see csview_substr_ex()
csview         cstr_u8_substr(const cstr* self, intptr_t bytepos, intptr_t u8len);
```
#### Iterate tokens with *c_fortoken*, *c_fortoken_sv*

Iterate tokens in an input string split by a separator string:
- `c_fortoken (it, const char* separator, const char* input_str);`
- `c_fortoken_sv (it, const char* separator, csview input_sv);`
- `it.token` is a csview of the current token.

```c
c_fortoken (i, ", ", "hello, one, two, three")
    printf("'%.*s' ", c_SVARG(i.token));
// 'hello' 'one' 'two' 'three'
```

#### Helper methods
```c
int            csview_cmp(const csview* x, const csview* y);
int            csview_icmp(const csview* x, const csview* y);
bool           csview_eq(const csview* x, const csview* y);
uint64_t       csview_hash(const csview* x);
```

## Types

| Type name       | Type definition                            | Used to represent...     |
|:----------------|:-------------------------------------------|:-------------------------|
| `csview`        | `struct { const char *buf; intptr_t size; }` | The string view type   |
| `csview_value`  | `const char`                               | The string element type  |
| `csview_iter`   | `union { csview_value *ref; csview chr; }` | UTF8 iterator            |

## Constants and macros

| Name           | Value                | Usage                                        |
|:---------------|:---------------------|:---------------------------------------------|
| `c_SVARG(sv)`     | printf argument      | `printf("sv: %.*s\n", c_SVARG(sv));`            |

## Example
```c
#define i_implement
#include "stc/cstr.h"
#include "stc/csview.h"

int main(void)
{
    cstr str1 = cstr_from("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csview ss1 = cstr_substr_ex(&str1, 3, 5);          // "think"
    intptr_t pos = cstr_find(&str1, "live");            // position of "live" in str1
    csview ss2 = cstr_substr_ex(&str1, pos, 4);        // get "live"
    csview ss3 = cstr_slice_ex(&str1, -8, -1);         // get "details"
    printf("%.*s %.*s %.*s\n",
        c_SVARG(ss1), c_SVARG(ss2), c_SVARG(ss3));
    cstr s1 = cstr_lit("Apples are red");
    cstr s2 = cstr_from_sv(cstr_substr_ex(&s1, -3, 3)); // "red"
    cstr s3 = cstr_from_sv(cstr_substr_ex(&s1, 0, 6));  // "Apples"
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

    cstr_u8_replace_at(&s1, cstr_find(&s1, "😀rld"), 1, c_sv("ø"));
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
        printf("%.*s,", c_SVARG(i.chr));

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
    c_fortoken_sv (i, input, sep)
        printf("[%.*s]\n", c_SVARG(i.token));
    puts("");
}
#define i_implement
#include "stc/cstr.h"
#define i_key_str
#include "stc/stack.h"

stack_str string_split(csview input, const char* sep)
{
    stack_str out = {0};

    c_fortoken_sv (i, input, sep)
        stack_str_push(&out, cstr_from_sv(i.token));

    return out;
}

int main(void)
{
    print_split(c_sv("//This is a//double-slash//separated//string"), "//");
    print_split(c_sv("This has no matching separator"), "xx");

    stack_str s = string_split(c_sv("Split,this,,string,now,"), ",");

    c_foreach (i, stack_str, s)
        printf("[%s]\n", cstr_str(i.ref));
    puts("");

    stack_str_drop(&s);
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
