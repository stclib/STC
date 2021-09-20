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
#include <stc/csview.h>
```
## Methods

```c
csview        csview_from(const char* str);                         // make csview from const char*
csview        csview_from_n(const char* str, size_t n);             // construct 
csview        csview_from_s(cstr s);                                // same as cstr_sv()

csview        csview_lit(const char literal_only[]);                // make csview from literal, no strlen()
csview        c_sv(const char literal_only[]);                      // same as csview_lit()

size_t        csview_size(csview sv);
size_t        csview_length(csview sv);
bool          csview_empty(csview sv);
char          csview_front(csview sv);
char          csview_back(csview sv);

void          csview_clear(csview* self);

csview        csview_substr(csview sv, intptr_t pos, size_t n);    // negative pos count from end
csview        csview_slice(csview sv, intptr_t p1, intptr_t p2);   // negative p1, p2 count from end
csview        csview_first_token(csview sv, csview sep);           // see split example below.
csview        csview_next_token(csview sv, csview sep, csview token);

bool          csview_equalto(csview sv, csview sv2);
size_t        csview_find(csview sv, csview needle);
bool          csview_contains(csview sv, csview needle);
bool          csview_starts_with(csview sv, csview sub);
bool          csview_ends_with(csview sv, csview sub);

csview_iter_t csview_begin(const csview* self);
csview_iter_t csview_end(const csview* self);
void          csview_next(csview_iter_t* it);
```
#### Extended cstr methods
```c
cstr          cstr_from_v(csview sv);                         // construct cstr from csview
cstr          cstr_from_replace_all_v(csview sv, csview find, csview replace);

csview        cstr_sv(cstr s);                                // convert to csview from cstr
csview        cstr_to_v(const cstr* self);                    // convert to csview from cstr*
csview        cstr_substr(cstr s, intptr_t pos, size_t n);    // negative pos counts from end
csview        cstr_slice(cstr s, intptr_t p1, intptr_t p2);   // negative p1, p2 counts from end

cstr*         cstr_assign_v(cstr* self, csview sv);
cstr*         cstr_append_v(cstr* self, csview sv);
void          cstr_insert_v(cstr* self, size_t pos, csview sv);
void          cstr_replace_v(cstr* self, size_t pos, size_t len, csview sv);

bool          cstr_equals_v(cstr s, csview sv);
size_t        cstr_find_v(cstr s, csview needle);
bool          cstr_contains_v(cstr s, csview needle);
bool          cstr_starts_with_v(cstr s, csview sub);
bool          cstr_ends_with_v(cstr s, csview sub);
```
#### Helper methods
```c
int           csview_compare(const csview* x, const csview* y);
bool          csview_equals(const csview* x, const csview* y);
uint64_t      csview_hash(const csview* x, ...);
```
## Types

| Type name         | Type definition                           | Used to represent...     |
|:------------------|:------------------------------------------|:-------------------------|
| `csview`          | `struct { const char *str; size_t size }` | The string view type     |
| `csview_value_t`  | `char`                                    | The string element type  |
| `csview_iter_t`   | `struct { csview_value_t *ref; }`         | csview iterator          |

## Constants and macros

| Name             | Value               | Usage                             |
|:-----------------|:--------------------|:----------------------------------|
| `csview_null`    | same as `c_sv("")`  | `sview = csview_null;`            |
| `csview_npos`    | same as `cstr_npos` |                                   |
| `csview_ARG(sv)` | printf argument     | `printf("%.*s", csview_ARG(sv));` |

## Example
```c
#include <stc/csview.h>

int main ()
{
    cstr str1 = cstr_lit("We think in generalities, but we live in details.");
                                                    // (quoting Alfred N. Whitehead)

    csview sv1 = cstr_substr(str1, 3, 5);           // "think"
    size_t pos = cstr_find(str1, "live");           // position of "live" in str1
    csview sv2 = cstr_substr(str1, pos, 4);         // get "live"
    csview sv3 = cstr_slice(str1, -8, -1);          // get "details"
    printf("%.*s %.*s %.*s\n", csview_ARG(sv1), csview_ARG(sv2), csview_ARG(sv3));

    cstr s1 = cstr_lit("Apples are red");
    cstr s2 = cstr_from_v(cstr_substr(s1, -3, 3));  // "red"
    cstr s3 = cstr_from_v(cstr_substr(s1, 0, 6));   // "Apples"
    printf("%s %s\n", s2, s3.str);

    c_del(cstr, &str1, &s1, &s2, &s3);
}
```
Output:
```
think live details
red Apples
```

### Example 2: csview tokenizer (string split)
Splits strings into tokens. *print_split()* makes **no** memory allocations or *strlen()* calls,
and does not depend on null-terminated strings. *string_split()* function returns a vector of cstr.
```c
#include <stc/csview.h>

void print_split(csview str, csview sep)
{
    csview token = csview_first_token(str, sep);
    for (;;) {
        // print non-null-terminated csview
        printf("\"%.*s\"\n", csview_ARG(token));
        if (csview_end(&token).ref == csview_end(&str).ref) break;
        token = csview_next_token(str, sep, token);
    }
}

#define i_val_str
#include <stc/cvec.h>

cvec_str string_split(csview str, csview sep)
{
    cvec_str vec = cvec_str_init();
    csview token = csview_first_token(str, sep);
    for (;;) {
        cvec_str_push_back(&vec, cstr_from_v(token));
        if (csview_end(&token).ref == csview_end(&str).ref) break;
        token = csview_next_token(str, sep, token);
    }
    return vec;
}

int main()
{
    print_split(c_sv("//This is a//double-slash//separated//string"), c_sv("//"));
    puts("");
    print_split(c_sv("This has no matching separator"), c_sv("xx"));
    puts("");

    c_autovar (cvec_str v = string_split(c_sv("Split,this,,string,now,"), c_sv(",")), cvec_str_del(&v))
        c_foreach (i, cvec_str, v)
            printf("\"%s\"\n", i.ref->str);
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

