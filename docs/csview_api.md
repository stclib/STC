# STC [csview](../include/stc/csview.h): Sub-string View
![String](pics/string.jpg)

The type **csview** is a non-null terminated string view and can refer to a constant contiguous
sequence of char-elements with the first element of the sequence at position zero. The implementation
holds two members: a pointer to constant char and a size.

Because **csview** is non-null terminated, it cannot be a replacent view for `const char*` - 
see [crawstr](crawstr_api.md) for that. **csview** never allocates memory, and therefore need not be
destructed. Its lifetime is limited by the source string storage. It keeps the length of the string,
and does not need to call *strlen()* to acquire the length.

- **csview** iterators works on UTF8 codepoints - like **cstr** and **crawstr** (see Example 2).
- Because it is null-terminated, it must be printed the following way:
```c
printf("%.*s", c_SV(sstr));
```

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
for a functional description.

## Header file

All csview definitions and prototypes are available by including a single header file.

```c
#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h> // after cstr.h: include extra cstr-csview functions
```
## Methods

```c
csview         c_sv(const char literal_only[]);                         // construct from literal, no strlen()
csview         c_sv(const char* str, intptr_t n);                       // construct from str and length n
csview         csview_from(const char* str);                           // construct from const char*
csview         csview_from_n(const char* str, intptr_t n);             // alias for c_sv(str, n)

intptr_t       csview_size(csview sv);
bool           csview_empty(csview sv);
void           csview_clear(csview* self);

bool           csview_equals(csview sv, const char* str);
intptr_t       csview_equals_sv(csview sv, csview find);
intptr_t       csview_find(csview sv, const char* str);
intptr_t       csview_find_sv(csview sv, csview find);
bool           csview_contains(csview sv, const char* str);
bool           csview_starts_with(csview sv, const char* str);
bool           csview_ends_with(csview sv, const char* str);
csview         csview_substr(csview sv, intptr_t pos, intptr_t n);
csview         csview_slice(csview sv, intptr_t pos1, intptr_t pos2);

csview         csview_substr_ex(csview sv, intptr_t pos, intptr_t n);     // negative pos count from end
csview         csview_slice_ex(csview sv, intptr_t pos1, intptr_t pos2);  // negative pos1, pos2 count from end
csview         csview_token(csview sv, const char* sep, intptr_t* start); // *start > sv.size after last token
```

#### UTF8 methods
```c
intptr_t       csview_u8_size(csview sv);
csview         csview_u8_substr(csview sv, intptr_t bytepos, intptr_t u8len);
bool           csview_valid_utf8(csview sv);                              // requires linking with src/utf8code.c
 
csview_iter    csview_begin(const csview* self);
csview_iter    csview_end(const csview* self);
void           csview_next(csview_iter* it);                              // utf8 codepoint step, not byte!
csview_iter    csview_advance(csview_iter it, intptr_t n);
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

To iterate tokens in an input string separated by a string:
```c
c_fortoken (i, "hello, one, two, three", ", ")
    printf("token: %.*s\n", c_SV(i.token));
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
| `c_SV(sv)`     | printf argument      | `printf("sv: %.*s\n", c_SV(sv));`            |

## Example
```c
#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h>

int main(void)
{
    cstr str1 = cstr_from("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csview ss1 = cstr_substr_ex(&str1, 3, 5);          // "think"
    intptr_t pos = cstr_find(&str1, "live");            // position of "live" in str1
    csview ss2 = cstr_substr_ex(&str1, pos, 4);        // get "live"
    csview ss3 = cstr_slice_ex(&str1, -8, -1);         // get "details"
    printf("%.*s %.*s %.*s\n",
        c_SV(ss1), c_SV(ss2), c_SV(ss3));
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
#include <stc/cstr.h>

int main(void)
{
    cstr s1 = cstr_lit("hell😀 w😀rld");

    cstr_u8_replace_at(&s1, cstr_find(&s1, "😀rld"), 1, c_sv("ø"));
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
        printf("%.*s,", c_SV(i.chr));

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
#define i_implement
#include <stc/cstr.h>
#define i_key_str
#include <stc/cstack.h>

cstack_str string_split(csview input, const char* sep)
{
    cstack_str out = cstack_str_init();
    
    c_fortoken_sv (i, input, sep)
        cstack_str_push(&out, cstr_from_sv(i.token));

    return out;
}

int main(void)
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
