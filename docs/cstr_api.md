# STC [cstr](../include/stc/cstr.h): String
![String](pics/string.jpg)

A **cstr** object represent sequences of characters. It supports an interface similar
to that of a standard container of bytes, but adding features specifically designed to
operate with strings of single-byte characters, terminated by the null character.

**cstr** has support for *UTF8* encoded strings, and has a set of small and
efficient functions for handling case-conversion, iteration and indexing into UTF8
codepoints (runes).

**cstr** uses short strings optimization (sso), which eliminates heap memory allocation
for string capacity up to 22 bytes.

## Header file

All cstr definitions and prototypes are available by including a single header file.

```c++
#include "stc/cstr.h"
```

## Methods
```c++
cstr            cstr_lit(const char literal_only[]);                    // cstr from literal; no strlen() call.
cstr            cstr_init(void);                                        // make an empty string
cstr            cstr_from(const char* str);                             // construct from a zero-terminated c-string.
cstr            cstr_from_s(cstr s, isize pos, isize len);              // construct a substring
cstr            cstr_from_sv(csview sv);                                // construct from a string view
cstr            cstr_from_zv(zsview zv);                                // construct from a zero-terminated zsview
cstr            cstr_from_fmt(const char* fmt, ...);                    // construct from printf() formatting
cstr            cstr_from_replace(csview sv, csview search, csview repl, int32_t count);
cstr            cstr_with_n(const char* str, isize n);                  // construct from first n bytes of str
cstr            cstr_with_capacity(isize cap);                          // make empty string with pre-allocated capacity.
cstr            cstr_with_size(isize len, char fill);                   // make string with fill characters

cstr            cstr_clone(cstr s);
cstr*           cstr_take(cstr* self, cstr s);                          // take ownership of s, i.e. don't drop s.
cstr            cstr_move(cstr* self);                                  // move string to caller, leave self empty
void            cstr_drop(cstr* self);                                  // destructor

zsview          cstr_zv(const cstr* self);                              // to zero-terminated string view
csview          cstr_sv(const cstr* self);                              // to csview string view
zsview          cstr_tail(cstr* self, isize len);                       // zsview subview of the trailing len bytes
csview          cstr_subview(const cstr* self, isize pos, isize len);   // csview subview from pos and length len

const char*     cstr_str(const cstr* self);                             // to const char*
char*           cstr_data(cstr* self);                                  // to mutable char*
cstr_view       cstr_getview(cstr* self);                               // to mutable buffer struct (with capacity)

isize           cstr_size(const cstr* self);
isize           cstr_capacity(const cstr* self);
isize           cstr_to_index(const cstr* self, cstr_iter it);          // get byte position at iter.
bool            cstr_is_empty(const cstr* self);                        // test from empty string

void            cstr_clear(cstr* self);
char*           cstr_reserve(cstr* self, isize capacity);               // return pointer to buffer
void            cstr_resize(cstr* self, isize len, char fill);
void            cstr_shrink_to_fit(cstr* self);

char*           cstr_assign(cstr* self, const char* str);
char*           cstr_assign_n(cstr* self, const char* str, isize n);    // assign n first bytes of str
char*           cstr_assign_sv(cstr* self, csview sv);
char*           cstr_copy(cstr* self, cstr s);                          // assign a clone of s
int             cstr_printf(cstr* self, const char* fmt, ...);          // source and target must not overlap.

char*           cstr_append(cstr* self, const char* str);
char*           cstr_append_n(cstr* self, const char* str, isize n);    // append n first bytes of str
char*           cstr_append_sv(cstr* self, csview str);
char*           cstr_append_s(cstr* self, cstr str);
int             cstr_append_fmt(cstr* self, const char* fmt, ...);      // printf() formatting
char*           cstr_append_uninit(cstr* self, isize len);              // return ptr to start of uninited data

void            cstr_join(cstr* self, const char* sep, cstr-vec vec);   // join and append vec/stack of cstrs
void            cstr_join_n(cstr* self, const char* sep, const char* arr[], isize n); // join and append c-strings
void            cstr_join_items(cstr* self, const char* sep, {const char* s1,...});   // join and append c-strings

void            cstr_push(cstr* self, const char* chr);                 // append one utf8 char
void            cstr_pop(cstr* self);                                   // pop one utf8 char

void            cstr_insert(cstr* self, isize pos, const char* ins);
void            cstr_insert_sv(cstr* self, isize pos, csview ins);

void            cstr_erase(cstr* self, isize pos, isize len);           // erase len bytes from pos

void            cstr_replace(cstr* self, const char* search, const char* repl);
void            cstr_replace_nfirst(cstr* self, const char* search, const char* repl, int32_t count); // replace count instances
cstr            cstr_replace_sv(csview in, csview search, csview repl, int32_t count);
void            cstr_replace_at(cstr* self, isize pos, isize len, const char* repl); // replace at a pos
void            cstr_replace_at_sv(cstr* self, isize pos, isize len, const csview repl);

bool            cstr_equals(const cstr* self, const char* str);
bool            cstr_equals_sv(const cstr* self, csview sv);

isize           cstr_find(const cstr* self, const char* search);
isize           cstr_find_sv(const cstr* self, csview search);
isize           cstr_find_at(const cstr* self, isize pos, const char* search); // search from pos
bool            cstr_contains(const cstr* self, const char* search);

bool            cstr_starts_with(const cstr* self, const char* str);
bool            cstr_starts_with_sv(const cstr* self, csview sv);

bool            cstr_ends_with(const cstr* self, const char* str);
bool            cstr_ends_with_sv(const cstr* self, csview sv);

bool            cstr_getline(cstr *self, FILE *stream);                 // cstr_getdelim(self, '\n', stream)
bool            cstr_getdelim(cstr *self, int delim, FILE *stream);     // does not append delim to result
```

#### UTF8 methods
```c++
cstr            cstr_u8_from(const char* str, isize u8pos, isize u8len);// make cstr from an utf8 substring
isize           cstr_u8_size(const cstr* self);                         // number of utf8 runes
isize           cstr_u8_to_index(const cstr* self, isize u8pos);        // get byte index at rune position
cstr_iter       cstr_u8_at(const cstr* self, isize u8pos);              // get rune at rune position
csview          cstr_u8_subview(const cstr* self, isize u8pos, isize u8len);
zsview          cstr_u8_tail(cstr* self, isize u8len);                  // subview of the trailing len runes
void            cstr_u8_insert(cstr* self, isize u8pos, const char* ins);
void            cstr_u8_replace(cstr* self, isize u8pos, isize u8len, const char* repl);
void            cstr_u8_erase(cstr* self, isize u8pos, isize u8len);    // erase u8len runes from u8pos
bool            cstr_u8_valid(const cstr* self);                        // verify that str is valid utf8

bool            cstr_iequals(const cstr* self, const char* str);        // utf8 case-insensitive comparison
bool            cstr_istarts_with(const cstr* self, const char* str);   // utf8 case-insensitive
bool            cstr_iends_with(const cstr* self, const char* str);     // utf8 case-insensitive

cstr_iter       cstr_begin(const cstr* self);                           // iterate utf8 codepoints (runes)
cstr_iter       cstr_end(const cstr* self);
void            cstr_next(cstr_iter* it);                               // next rune
cstr_iter       cstr_advance(cstr_iter it, isize u8pos);                // advance +/- runes

cstr            cstr_casefold_sv(csview sv);                            // returns new casefolded utf8 cstr
cstr            cstr_tolower_sv(csview sv);                             // returns new lowercase utf8 cstr
cstr            cstr_toupper_sv(csview sv);                             // returns new uppercase utf8 cstr
cstr            cstr_tolower(const char* str);                          // returns new lowercase utf8 cstr
cstr            cstr_toupper(const char* str);                          // returns new uppercase utf8 cstr
void            cstr_lowercase(cstr* self);                             // transform cstr to lowercase utf8
void            cstr_uppercase(cstr* self);                             // transform cstr to uppercase utf8
```

Note that all methods with arguments `(..., const char* str, isize n)`, `n` must be within the range of `str` length.

#### Helper methods:
```c++
size_t          cstr_hash(const cstr* self);
int             cstr_cmp(const cstr* s1, const cstr* s2);
bool            cstr_eq(const cstr* s1, const cstr* s2);
int             cstr_icmp(const cstr* s1, const cstr* s2);              // utf8 case-insensitive comparison
bool            cstr_ieq(const cstr* s1, const cstr* s2);               // utf8 case-insensitive comparison

char*           c_strnstrn(const char* str, isize slen, const char* needle, isize nlen);
```

## Types

| Type name       | Type definition                              | Used to represent... |
|:----------------|:---------------------------------------------|:---------------------|
| `cstr`          | `struct { ... }`                             | The string type      |
| `cstr_value`    | `char`                                       | String element type  |
| `cstr_iter`     | `union { cstr_value *ref; csview chr; }`     | String iterator      |
| `cstr_view`     | `struct { char *data; isize size, cap; }` | String buffer type   |

## Constants and macros

| Name            | Value             |
|:----------------|:------------------|
|  `c_NPOS`       | `INTPTR_MAX`      |

## Example
```c++
#include "stc/cstr.h"

int main(void) {
    cstr s0, s1, full_path;
    c_defer(
        cstr_drop(&s0), cstr_drop(&s1), cstr_drop(&full_path)
    ){
        s0 = cstr_lit("Initialization without using strlen().");
        printf("%s\nLength: %" c_ZI "\n\n", cstr_str(&s0), cstr_size(&s0));

        s1 = cstr_lit("one-nine-three-seven-five.");
        printf("%s\n", cstr_str(&s1));

        cstr_insert(&s1, 3, "-two");
        printf("%s\n", cstr_str(&s1));

        cstr_erase(&s1, 7, 5); // -nine
        printf("%s\n", cstr_str(&s1));

        cstr_replace_nfirst(&s1, "seven", "four", 1);
        printf("%s\n", cstr_str(&s1));

        // reassign:
        cstr_assign(&s1, "one two three four five six seven");
        cstr_append(&s1, " eight");
        printf("append: %s\n", cstr_str(&s1));

        full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
        printf("%s\n", cstr_str(&full_path));
    }
}
```
Output:
```
one-nine-three-seven-five.
one-two-nine-three-seven-five.
one-two-three-seven-five.
one-two-three-four-five.
append: one two three four five six seven eight
directory/filename.ext
```
