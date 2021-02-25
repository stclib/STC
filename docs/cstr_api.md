# STC [cstr](../stc/cstr.h): String
![String](pics/string.jpg)

A **cstr** object represent sequences of characters. It supports an interface similar to that of a standard container of bytes, but adding features specifically designed to operate with strings of single-byte characters, terminated by the null character.

See the c++ class [std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string) for a functional description.

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cstr.h"
```
## Methods

```c
cstr         cstr_init(void);                                         // constructor
cstr         cstr_with_capacity(size_t cap);
cstr         cstr_with_size(size_t len, char fill);                   // repeat fill len times
cstr         cstr_from(const char* str);
cstr         cstr_from_n(const char* str, size_t n);
cstr         cstr_from_fmt(const char* fmt, ...);                     // printf() formatting

cstr         cstr_clone(cstr s);
void         cstr_del(cstr *self);                                    // destructor

size_t       cstr_size(cstr s);
size_t       cstr_length(cstr s);
size_t       cstr_capacity(cstr s);
bool         cstr_empty(cstr s);

size_t       cstr_reserve(cstr* self, size_t capacity);
void         cstr_resize(cstr* self, size_t len, char fill);
void         cstr_clear(cstr* self);
cstr*        cstr_assign(cstr* self, const char* str);
cstr*        cstr_assign_n(cstr* self, const char* str, size_t n);
cstr*        cstr_take(cstr* self, cstr s);                           // take the constructed or moved string
cstr         cstr_move(cstr* self);                                   // move string to caller, leave empty string

cstr*        cstr_append(cstr* self, const char* str);
cstr*        cstr_append_n(cstr* self, const char* str, size_t n);  // appends len characters
void         cstr_push_back(cstr* self, char ch);
void         cstr_pop_back(cstr* self);
void         cstr_insert(cstr* self, size_t pos, const char* str);
void         cstr_insert_n(cstr* self, size_t pos, const char* str, size_t n);
void         cstr_erase(cstr* self, size_t pos);
void         cstr_erase_n(cstr* self, size_t pos, size_t n);
void         cstr_replace(cstr* self, size_t pos, size_t len, const char* str);
void         cstr_replace_n(cstr* self, size_t pos, size_t len, const char* str, size_t n);

int          cstr_compare(const cstr *s1, const cstr *s2);
bool         cstr_equals(cstr s, const char* str);
bool         cstr_equals_s(cstr s, cstr s2);
bool         cstr_iequals(cstr s, const char* str);                   // prefix i = case-insensitive
size_t       cstr_find(cstr s, const char* substr);
size_t       cstr_find_n(cstr s, const char* substr, size_t pos, size_t n);
size_t       cstr_ifind_n(cstr s, const char* substr, size_t pos, size_t n);
bool         cstr_contains(cstr s, const char* substr);
bool         cstr_icontains(cstr s, const char* substr);
bool         cstr_begins_with(cstr s, const char* substr);
bool         cstr_ibegins_with(cstr s, const char* substr);
bool         cstr_ends_with(cstr s, const char* substr);
bool         cstr_iends_with(cstr s, const char* substr);

char*        cstr_front(cstr* self);
char*        cstr_back(cstr* self);

cstr_iter_t  cstr_begin(cstr* self);
cstr_iter_t  cstr_end(cstr* self);
void         cstr_next(cstr_iter_t* it);
char*        cstr_itval(cstr_iter_t it);

bool         cstr_getline(cstr *self, FILE *stream);                  // cstr_getdelim(self, '\n', stream)
bool         cstr_getdelim(cstr *self, int delim, FILE *stream);
```
```c
const char*  cstr_c_str(const cstr* x);
int          cstr_compare_raw(const char** x, const char** y);
bool         cstr_equals_raw(const char** x, const char** y);
uint32_t     cstr_hash_raw(const char* const* x, size_t ignored);
int          c_strncasecmp(const char* s1, const char* s2, size_t n);
char*        c_strnfind(const char* str, const char* needle, size_t n);
char*        c_istrnfind(const char* str, const char* needle, size_t n);
uint32_t     c_strhash(const char* str);
```
Helper methods, used by other container types.

## Types

| Type name         | Type definition                  | Used to represent...     |
|:------------------|:---------------------------------|:-------------------------|
| `cstr`            | `struct { const char *str; }`    | The string type          |
| `cstr_value_t`    | `char`                           | The string element type  |
| `cstr_iter_t`     | `struct { cstr_value_t *ref; }`  | cstr iterator            |

## Constants and macros

| Name              | Value            |
|:------------------|:-----------------|
|  `cstr_npos`      | `(-1ull)`        |

## Example
```c
#include <stc/cstr.h>

int main() {
    cstr s1 = cstr_from("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase_n(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&s1, cstr_find(&s1, "seven"), 5, "four");
    printf("%s.\n", s1.str);

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);

    cstr full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", full_path.str);

    c_del(cstr, &s1, &full_path);
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
