# STC [cstr](../include/stc/cstr.h): String
![String](pics/string.jpg)

A **cstr** object represent sequences of characters. It supports an interface similar to that of a standard container of bytes, but adding features specifically designed to operate with strings of single-byte characters, terminated by the null character.

**cstr** has basic support for *UTF8* encoded strings, and has a set of compact and efficient functions for handling case-foldings and comparisons of UTF strings. The **cstr** type uses short strings optimization (sso), which eliminates heap memory allocation for strings shorter than 24 bytes (sizeof(cstr) is also 24).

See the c++ class [std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string) for a functional description.

## Header file

All cstr definitions and prototypes are available by including a single header file.

```c
#define i_implement // optional: define in one source file only for shared symbols linking!
#include <stc/cstr.h>
```

## Methods
```c
cstr         cstr_init(void);                                         // constructor; same as cstr_null.
cstr         cstr_new(const char literal_only[]);                     // cstr from literal; no strlen() call.
cstr         cstr_from(const char* str);                              // constructor using strlen()
cstr         cstr_from_n(const char* str, size_t n);                  // constructor with specified length
cstr         cstr_with_capacity(size_t cap);
cstr         cstr_with_size(size_t len, char fill);                   // repeat fill len times
cstr         cstr_from_fmt(const char* fmt, ...);                     // printf() formatting
cstr         cstr_clone(cstr s);

cstr*        cstr_take(cstr* self, cstr s);                           // take the constructed or moved string
cstr         cstr_move(cstr* self);                                   // move string to caller, leave empty string
void         cstr_drop(cstr *self);                                   // destructor

const char*  cstr_str(const cstr* self);                              // access to const char*
char*        cstr_data(cstr* self);                                   // access to char*
csview       cstr_sv(const cstr* self);                               // access to string view
cstr_buf     cstr_buffer(cstr* self);                                 // access to mutable buffer (with capacity)

size_t       cstr_size(cstr s);
size_t       cstr_length(cstr s);
size_t       cstr_capacity(cstr s);
bool         cstr_empty(cstr s);

size_t       cstr_reserve(cstr* self, size_t capacity);
void         cstr_resize(cstr* self, size_t len, char fill);
void         cstr_shrink_to_fit(cstr* self);
char*        cstr_expand_uninit(cstr* self, size_t n);                // return ptr to uninit data
void         cstr_clear(cstr* self);

char*        cstr_assign(cstr* self, const char* str);
char*        cstr_assign_s(cstr* self, cstr s);
char*        cstr_assign_n(cstr* self, const char* str, size_t n);    // assign n first chars of str
void         cstr_copy(cstr* self, cstr s);                           // like cstr_assign_s()
int          cstr_printf(cstr* self, const char* fmt, ...);           // printf() formatting

char*        cstr_append(cstr* self, const char* app);
char*        cstr_append_s(cstr* self, cstr app);
char*        cstr_append_n(cstr* self, const char* app, size_t n);

void         cstr_insert(cstr* self, size_t pos, const char* ins);
void         cstr_insert_s(cstr* self, size_t pos, cstr ins);
void         cstr_insert_n(cstr* self, size_t pos, const char* ins, size_t n);

void         cstr_erase(cstr* self, size_t pos);
void         cstr_erase_n(cstr* self, size_t pos, size_t n);

size_t       cstr_replace(cstr* self, const char* search, const char* repl);
size_t       cstr_replace_from(cstr* self, size_t pos, const char* search, const char* repl);
void         cstr_replace_at(cstr* self, size_t pos, size_t len, const char* repl);
void         cstr_replace_s(cstr* self, size_t pos, size_t len, cstr repl);
void         cstr_replace_n(cstr* self, size_t pos, size_t len, const char* repl, size_t n);
void         cstr_replace_all(cstr* self, const char* search, const char* repl);

bool         cstr_equals(cstr s, const char* str);
bool         cstr_equals_s(cstr s, cstr s2);
size_t       cstr_find(cstr s, const char* search);
size_t       cstr_find_from(cstr s, size_t pos, const char* search);
bool         cstr_contains(cstr s, const char* search);
bool         cstr_starts_with(cstr s, const char* str);
bool         cstr_ends_with(cstr s, const char* str);

bool         cstr_getline(cstr *self, FILE *stream);                  // cstr_getdelim(self, '\n', stream)
bool         cstr_getdelim(cstr *self, int delim, FILE *stream);      // does not append delim to result
```

#### UTF8 methods
```c
size_t       cstr_size_u8(cstr s);                                    // number of utf8 codepoints
size_t       cstr_size_n_u8(cstr s, size_t nbytes);                   // utf8 size within n bytes  
csview       cstr_at(const cstr* self, size_t bytepos);               // utf8 codepoints as a csview
csview       cstr_at_u8(const cstr* self, size_t u8idx);              // utf8 codepoints at utf8 pos
size_t       cstr_pos_u8(const cstr* self, size_t u8idx);             // byte position at utf8 index

// iterate utf8 codepoints
cstr_iter    cstr_begin(const cstr* self);
cstr_iter    cstr_end(const cstr* self);
void         cstr_next(cstr_iter* it);

// utf8 functions requires linking with src/utf8code.c symbols:
bool         cstr_valid_u8(const cstr* self);                         // check if str is valid utf8
cstr         cstr_tolower(const cstr* self);                          // returns new lowercase utf8 cstr
cstr         cstr_toupper(const cstr* self);                          // returns new uppercase utf8 cstr
void         cstr_lowercase(cstr* self);                              // transform cstr to lowercase utf8
void         cstr_uppercase(cstr* self);                              // transform cstr to uppercase utf8
bool         cstr_iequals(cstr s, const char* str);                   // utf8 case-insensitive comparison
bool         cstr_istarts_with(cstr s, const char* str);              //   "
bool         cstr_iends_with(cstr s, const char* str);                //   "
```

Note that all methods with arguments `(..., const char* str, size_t n)`, `n` must be within the range of `str` length.

#### Helper methods:
```c
int          cstr_cmp(const cstr *s1, const cstr *s2);
int          cstr_icmp(const cstr* s1, const cstr* s2);               //  utf8 case-insensitive comparison
bool         cstr_eq(const cstr *s1, const cstr *s2);
bool         cstr_hash(const cstr *s);

char*        c_strnstrn(const char* str, const char* search, size_t slen, size_t nlen);
```

## Types

| Type name       | Type definition                            | Used to represent... |
|:----------------|:-------------------------------------------|:---------------------|
| `cstr`          | `struct { ... }`                           | The string type      |
| `cstr_value`    | `char`                                     | String element type  |
| `csview`        | `struct { const char *str; size_t size; }` | String view type     |
| `cstr_buf`      | `struct { char *data; size_t size, cap; }` | String buffer type   |

## Constants and macros

| Name              | Value             |
|:------------------|:------------------|
|  `cstr_npos`      | `INTPTR_MAX`      |
|  `cstr_null`      | cstr null value   |

## Example
```c
#include <stc/cstr.h>

int main() {
    cstr s0 = cstr_new("Initialization without using strlen().");
    printf("%s\nLength: %" PRIuMAX "\n\n", cstr_str(&s0), cstr_size(s0));

    cstr s1 = cstr_new("one-nine-three-seven-five.");
    printf("%s\n", cstr_str(&s1));

    cstr_insert(&s1, 3, "-two");
    printf("%s\n", cstr_str(&s1));

    cstr_erase_n(&s1, 7, 5); // -nine
    printf("%s\n", cstr_str(&s1));

    cstr_replace_at(&s1, cstr_find(s1, "seven"), 5, "four");
    printf("%s\n", cstr_str(&s1));

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", cstr_str(&s1));

    cstr full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", cstr_str(&full_path));

    c_drop(cstr, &s0, &s1, &full_path);
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
