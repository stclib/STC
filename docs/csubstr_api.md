# STC [csubstr](../include/stc/csubstr.h): Sub-string View
![String](pics/string.jpg)

The type **csubstr** is a non-null terminated string view and can refer to a constant contiguous sequence of
char-elements with the first element of the sequence at position zero. The implementation holds two members:
a pointer to constant char and a size.

Because **csubstr** is non-null terminated, it is not a replacent view for `const char*` - see [csview](csview_api.md)
for that. **csubstr** never allocates memory, and therefore need not be destructed. Its lifetime is limited by
the source string storage. It keeps the length of the string, and does not need to call *strlen()* to acquire
the length.

- **csubstr** iterators works on UTF8 codepoints - like **cstr** and **csview** (see Example 2).
- Because it is null-terminated, it must be printed the following way:
```c
printf("%.*s", c_SS(sstr));
```

See the c++ class [std::basic_string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) for a functional
description.

## Header file

All csubstr definitions and prototypes are available by including a single header file.

```c
#define i_implement
#include <stc/cstr.h>
#include <stc/csubstr.h> // after cstr.h: include extra cstr-csubstr functions
```
## Methods

```c
csubstr         c_ss(const char literal_only[]);                         // construct from literal, no strlen()
csubstr         c_ss(const char* str, intptr_t n);                       // construct from str and length n
csubstr         csubstr_from(const char* str);                           // construct from const char*
csubstr         csubstr_from_n(const char* str, intptr_t n);             // alias for c_ss(str, n)

intptr_t        csubstr_size(csubstr ss);
bool            csubstr_empty(csubstr ss);
void            csubstr_clear(csubstr* self);

bool            csubstr_equals(csubstr ss, const char* str);
intptr_t        csubstr_equals_ss(csubstr ss, csubstr find);
intptr_t        csubstr_find(csubstr ss, const char* str);
intptr_t        csubstr_find_ss(csubstr ss, csubstr find);
bool            csubstr_contains(csubstr ss, const char* str);
bool            csubstr_starts_with(csubstr ss, const char* str);
bool            csubstr_ends_with(csubstr ss, const char* str);
csubstr         csubstr_substr(csubstr ss, intptr_t pos, intptr_t n);
csubstr         csubstr_slice(csubstr ss, intptr_t pos1, intptr_t pos2);

csubstr         csubstr_substr_ex(csubstr ss, intptr_t pos, intptr_t n);     // negative pos count from end
csubstr         csubstr_slice_ex(csubstr ss, intptr_t pos1, intptr_t pos2);  // negative pos1, pos2 count from end
csubstr         csubstr_token(csubstr ss, const char* sep, intptr_t* start); // *start > ss.size after last token
```

#### UTF8 methods
```c
intptr_t        csubstr_u8_size(csubstr ss);
csubstr         csubstr_u8_substr(csubstr ss, intptr_t bytepos, intptr_t u8len);
bool            csubstr_valid_utf8(csubstr ss);                              // requires linking with src/utf8code.c
 
csubstr_iter    csubstr_begin(const csubstr* self);
csubstr_iter    csubstr_end(const csubstr* self);
void            csubstr_next(csubstr_iter* it);                              // utf8 codepoint step, not byte!
csubstr_iter    csubstr_advance(csubstr_iter it, intptr_t n);
```

#### cstr methods returning csubstr
```c
csubstr         cstr_slice(const cstr* self, intptr_t pos1, intptr_t pos2);
csubstr         cstr_slice_ex(const cstr* self, intptr_t pos1, intptr_t pos2); // see csubstr_slice_ex()
csubstr         cstr_substr(const cstr* self, intptr_t pos, intptr_t n);
csubstr         cstr_substr_ex(const cstr* self, intptr_t pos, intptr_t n);    // see csubstr_substr_ex()
csubstr         cstr_u8_substr(const cstr* self, intptr_t bytepos, intptr_t u8len);
```
#### Iterate tokens with *c_fortoken*, *c_fortoken_ss*

To iterate tokens in an input string separated by a string:
```c
c_fortoken (i, "hello, one, two, three", ", ")
    printf("token: %.*s\n", c_SS(i.token));
```

#### Helper methods
```c
int             csubstr_cmp(const csubstr* x, const csubstr* y);
int             csubstr_icmp(const csubstr* x, const csubstr* y);
bool            csubstr_eq(const csubstr* x, const csubstr* y);
uint64_t        csubstr_hash(const csubstr* x);
```

## Types

| Type name       | Type definition                            | Used to represent...     |
|:----------------|:-------------------------------------------|:-------------------------|
| `csubstr`        | `struct { const char *str; intptr_t size; }` | The string view type     |
| `csubstr_value`  | `char`                                     | The string element type  |
| `csubstr_iter`   | `struct { csubstr_value *ref; }`            | UTF8 iterator            |

## Constants and macros

| Name           | Value                | Usage                                        |
|:---------------|:---------------------|:---------------------------------------------|
| `c_SS(ss)`     | printf argument      | `printf("ss: %.*s\n", c_SS(ss));`            |

## Example
```c
#define i_implement
#include <stc/cstr.h>
#include <stc/csubstr.h>

int main(void)
{
    cstr str1 = cstr_from("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csubstr ss1 = cstr_substr_ex(&str1, 3, 5);          // "think"
    intptr_t pos = cstr_find(&str1, "live");            // position of "live" in str1
    csubstr ss2 = cstr_substr_ex(&str1, pos, 4);        // get "live"
    csubstr ss3 = cstr_slice_ex(&str1, -8, -1);         // get "details"
    printf("%.*s %.*s %.*s\n",
        c_SS(ss1), c_SS(ss2), c_SS(ss3));
    cstr s1 = cstr_lit("Apples are red");
    cstr s2 = cstr_from_ss(cstr_substr_ex(&s1, -3, 3)); // "red"
    cstr s3 = cstr_from_ss(cstr_substr_ex(&s1, 0, 6));  // "Apples"
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

    cstr_u8_replace_at(&s1, cstr_find(&s1, "😀rld"), 1, c_ss("ø"));
    printf("%s\n", cstr_str(&s1));

    c_foreach (i, cstr, s1)
        printf("%.*s,", c_SS(i.u8.chr));

    cstr_drop(&s1);
}
```
Output:
```
hell😀 wørld
h,e,l,l,😀, ,w,ø,r,l,d,
```

### Example 3: csubstr tokenizer (string split)
Splits strings into tokens. *print_split()* makes **no** memory allocations or *strlen()* calls,
and does not depend on null-terminated strings. *string_split()* function returns a vector of cstr.
```c
#include <stdio.h>
#include <stc/csubstr.h>

void print_split(csubstr input, const char* sep)
{
    c_fortoken_ss (i, input, sep)
        printf("[%.*s]\n", c_SS(i.token));
    puts("");
}
#define i_implement
#include <stc/cstr.h>
#define i_key_str
#include <stc/cstack.h>

cstack_str string_split(csubstr input, const char* sep)
{
    cstack_str out = cstack_str_init();
    
    c_fortoken_ss (i, input, sep)
        cstack_str_push(&out, cstr_from_ss(i.token));

    return out;
}

int main(void)
{
    print_split(c_ss("//This is a//double-slash//separated//string"), "//");
    print_split(c_ss("This has no matching separator"), "xx");

    cstack_str s = string_split(c_ss("Split,this,,string,now,"), ",");

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
