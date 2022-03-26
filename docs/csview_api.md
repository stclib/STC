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
csview          c_sv(const char literal_only[]);                    // alias for csview_new
csview          csview_new(const char literal_only[]);              // make csview from literal, no strlen()
csview          csview_from_s(const cstr* s);                       // convert to csview from cstr
csview          csview_from(const char* str);                       // make csview from const char*
csview          csview_from_n(const char* str, size_t n);           // construct 

size_t          csview_size(csview sv);
size_t          csview_length(csview sv);
bool            csview_empty(csview sv);
char            csview_front(csview sv);
char            csview_back(csview sv);

void            csview_clear(csview* self);

csview          csview_substr(csview sv, intptr_t pos, size_t n);   // negative pos count from end
csview          csview_slice(csview sv, intptr_t p1, intptr_t p2);  // negative p1, p2 count from end
csview          csview_token(csview sv, csview sep, size_t* start); // see split example below.

bool            csview_equals(csview sv, csview sv2);
size_t          csview_find(csview sv, csview needle);
bool            csview_contains(csview sv, csview needle);
bool            csview_starts_with(csview sv, csview sub);
bool            csview_ends_with(csview sv, csview sub);

csview_iter     csview_begin(const csview* self);
csview_iter     csview_end(const csview* self);
void            csview_next(csview_iter* it);                       // NB: UTF8 codepoint step, not byte!
```

#### UTF8 methods
```
bool            utf8_valid_sv(csview sv);
size_t          utf8_size_sv(csview sv);
csview          utf8_substr(const char* str, size_t pos, size_t n);

bool            utf8_valid(const char* s);
size_t          utf8_size(const char *s);
size_t          utf8_size_n(const char *s, size_t n);               // number of UTF8 codepoints within n bytes
const char*     utf8_at(const char *s, size_t index);               // from UTF8 index to char* position
size_t          utf8_pos(const char* s, size_t index);              // from UTF8 index to byte index position
const char*     utf8_next(const char *s);                           // next codepoint as char*; NULL if *s == 0
uint32_t        utf8_peek(const char *s);                           // next codepoint as uint32_t

size_t          utf8_codep_size(const char* s);                     // 1-4 (0 if s[0] is illegal first cp char)
uint32_t        utf8_decode(uint32_t *state, uint32_t *codep, const uint32_t byte); // decode next utf8 codepoint.
```

#### Extended cstr methods
```c
cstr            cstr_from_sv(csview sv);                            // construct cstr from csview
csview          cstr_to_sv(const cstr* self);                       // convert to csview from const cstr*
cstr            cstr_from_replace_all_sv(csview sv, csview find, csview replace);

csview          cstr_substr(const cstr* s, intptr_t pos, size_t n); // negative pos count from end
csview          cstr_slice(const cstr* s, intptr_t p, intptr_t q);  // negative p or q count from end

cstr*           cstr_assign_sv(cstr* self, csview sv);
cstr*           cstr_append_sv(cstr* self, csview sv);
void            cstr_insert_sv(cstr* self, size_t pos, csview sv);
void            cstr_replace_sv(cstr* self, size_t pos, size_t len, csview sv);

bool            cstr_equals_sv(cstr s, csview sv);
size_t          cstr_find_sv(cstr s, csview needle);
bool            cstr_contains_sv(cstr s, csview needle);
bool            cstr_starts_with_sv(cstr s, csview sub);
bool            cstr_ends_with_sv(cstr s, csview sub);
```

#### Helper methods
```c
int             csview_cmp(const csview* x, const csview* y);
bool            csview_eq(const csview* x, const csview* y);
uint64_t        csview_hash(const csview* x, size_t dummy);
```

## Types

| Type name       | Type definition                           | Used to represent...     |
|:----------------|:------------------------------------------|:-------------------------|
| `csview`        | `struct { const char *str; size_t size }` | The string view type     |
| `csview_value`  | `char`                                    | The string element type  |
| `csview_iter`   | `struct { csview_value *ref; }`           | UTF8 iterator            |

## Constants and macros

| Name           | Value                | Usage                                         |
|:---------------|:---------------------|:----------------------------------------------|
| `csview_null`  | same as `c_sv("")`   | `sview = csview_null;`                        |
| `csview_npos`  | same as `cstr_npos`  |                                               |
| `c_PRIsv`      | printf format csview |                                               |
| `c_ARGsv(sv)`  | printf argument      | `printf("view: " c_PRIsv "\n", c_ARGsv(sv));` |

## Example
```c
#include <stc/cstr.h>
#include <stc/csview.h>

int main ()
{
    cstr str1 = cstr_new("We think in generalities, but we live in details.");
                                                        // (quoting Alfred N. Whitehead)

    csview sv1 = cstr_substr(&str1, 3, 5);              // "think"
    size_t pos = cstr_find(str1, "live");               // position of "live" in str1
    csview sv2 = cstr_substr(&str1, pos, 4);            // get "live"
    csview sv3 = cstr_slice(&str1, -8, -1);             // get "details"
    printf(c_PRIsv c_PRIsv c_PRIsv "\n", c_ARGsv(sv1), c_ARGsv(sv2), c_ARGsv(sv3));

    cstr s1 = cstr_new("Apples are red");
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
    c_auto (cstr, s1) {
        s1 = cstr_new("hell😀 w😀rld");
        cstr_replace_sv(&s1, utf8_substr(cstr_str(&s1), 7, 1), c_sv("x"));
        printf("%s\n", cstr_str(&s1));

        csview sv = csview_from_s(&s1);
        c_foreach (i, csview, sv)
            printf(c_PRIsv ",", c_ARGsv(i.codep));
    }
}
```
Output:
```
hell😀 wxrld
h,e,l,l,😀, ,w,x,r,l,d,
```

### Example 3: csview tokenizer (string split)
Splits strings into tokens. *print_split()* makes **no** memory allocations or *strlen()* calls,
and does not depend on null-terminated strings. *string_split()* function returns a vector of cstr.
```c
#include <stc/csview.h>

void print_split(csview str, csview sep)
{
    size_t pos = 0;
    while (pos != str.size) {
        csview tok = csview_token(str, sep, &pos);
        // print non-null-terminated csview
        printf("[" c_PRIsv "]\n", c_ARGsv(tok));
    }
}

#define i_val_str
#include <stc/cvec.h>
#include <stc/csview.h>

cvec_str string_split(csview str, csview sep)
{
    cvec_str vec = cvec_str_init();
    size_t pos = 0;
    while (pos != str.size) {
        csview tok = csview_token(str, sep, &pos);
        cvec_str_push_back(&vec, cstr_from_sv(tok));
    }
    return vec;
}

int main()
{
    print_split(c_sv("//This is a//double-slash//separated//string"), c_sv("//"));
    puts("");
    print_split(c_sv("This has no matching separator"), c_sv("xx"));
    puts("");

    c_autovar (cvec_str v = string_split(c_sv("Split,this,,string,now,"), c_sv(",")), cvec_str_drop(&v))
        c_foreach (i, cvec_str, v)
            printf("[%s]\n", i.ref->str);
}
```
Output:
```
""
"This is a"
"double-slash"
"separated"
"string"

"This has no matching separator"

"Split"
"this"
""
"string"
"now"
""
