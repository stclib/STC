# STC Container [cstr](../stc/cstr.h): String
![String](pics/string.jpg)

This describes the API of string type **cstr**.
See [std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string) for a similar c++ class.

## Types

| Type name         | Type definition                  | Used to represent...     |
|:------------------|:---------------------------------|:-------------------------|
| `cstr, cstr_t`    | `struct { const char *str; }`    | The string type          |
| `cstr_value_t`    | `char`                           | The string element type  |
| `cstr_iter_t`     | `struct { cstr_value_t *ref; }`  | cstr iterator            |

## Constants and macros

| Name              | Value            |
|:------------------|:-----------------|
|  `cstr_inits`     | `{...}`          |
|  `cstr_npos`      | `-1ull`          |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cstr.h"
```
## Methods

```c
 1)     cstr         cstr_init(void);
 2)     cstr         cstr_with_capacity(size_t cap);
 3)     cstr         cstr_with_size(size_t len, char fill);
 4)     cstr         cstr_from(const char* str);
        cstr         cstr_from_n(const char* str, size_t len);
 5)     cstr         cstr_from_fmt(const char* fmt, ...);
 6)     cstr         cstr_clone(cstr s);
 7)     void         cstr_del(cstr *self);
```
`1)` Create an empty string, `2)` with capacity. `3)` Create a string by repeating *fill* character *len* times.
`4)` Construct a string from *str*, limited by *len* and *strlen(str)*.
`5)` Construct a string from the format specified by *fmt* and arguments, using *printf()* formatting.
`6)` Construct a new string by cloning another string. `7)` Destruct/free the allocated memory used by string.
```c
        size_t       cstr_size(cstr s);
        size_t       cstr_length(cstr s);
        size_t       cstr_capacity(cstr s);
        bool         cstr_empty(cstr s);
 5)     char*        cstr_front(cstr* self);
 6)     char*        cstr_back(cstr* self);
```
These returns properties of a string. `5-6)` returns reference, ie. pointer to the char.
```c
 1)     size_t       cstr_reserve(cstr* self, size_t capacity);
        void         cstr_resize(cstr* self, size_t len, char fill);
        void         cstr_clear(cstr* self);
 2)     cstr*        cstr_assign(cstr* self, const char* str);
        cstr*        cstr_assign_n(cstr* self, const char* str, size_t len);
 3)     cstr*        cstr_take(cstr* self, cstr s);
 4)     cstr         cstr_move(cstr* self);
```
`1)` Reserve, resize, clear string. `2)` Assign *str* limited by *len* to string. 
`3)` Take the constructed or moved string *s*, i.e., no allocation takes place.
`4)` Explicitly move string to the caller of the method; string becomes empty after move.
```c
 1)     cstr*        cstr_append(cstr* self, const char* str);
        cstr*        cstr_append_n(cstr* self, const char* str, size_t len);
 2)     void         cstr_push_back(cstr* self, char ch);
        void         cstr_pop_back(cstr* self);
 3)     void         cstr_insert(cstr* self, size_t pos, const char* str);
        void         cstr_insert_n(cstr* self, size_t pos, const char* str, size_t n);
 4)     void         cstr_erase(cstr* self, size_t pos);
        void         cstr_erase_n(cstr* self, size_t pos, size_t n);
 5)     void         cstr_replace(cstr* self, size_t pos, size_t len, const char* str);
        void         cstr_replace_n(cstr* self, size_t pos, size_t len, const char* str, size_t n);
```
`1)` Append *str* to string limited by *len*. `2)` Append / remove last character *ch*. 
`3)` Insert string at a position *pos* limited by *n* characters. `4)` Erase *n* characters at position *pos*.
`5)` Replace *len* characters at position *pos* with *str* limited by *n* characters.
```c
        int          cstr_compare(const cstr *s1, const cstr *s2);
        bool         cstr_equals(cstr s, const char* str);
        bool         cstr_equals_s(cstr s, cstr s2);
        bool         cstr_iequals(cstr s, const char* str);
        size_t       cstr_find(cstr s, const char* substr);
        size_t       cstr_find_n(cstr s, const char* substr, size_t pos, size_t nlen);
        size_t       cstr_ifind_n(cstr s, const char* substr, size_t pos, size_t nlen);
        bool         cstr_contains(cstr s, const char* substr);
        bool         cstr_icontains(cstr s, const char* substr);
        bool         cstr_begins_with(cstr s, const char* substr);
        bool         cstr_ibegins_with(cstr s, const char* substr);
        bool         cstr_ends_with(cstr s, const char* substr);
        bool         cstr_iends_with(cstr s, const char* substr);
```
Compare and search methods. Methods prefixed by *i* does case-insensitive compare/search.
```c
        cstr_iter_t  cstr_begin(cstr* self);
        cstr_iter_t  cstr_end(cstr* self);
        void         cstr_next(cstr_iter_t* it);
        char*        cstr_itval(cstr_iter_t it);
```
Iterator methods, typically used via the general *c_foreach* macro.
```c
 1)     bool         cstr_getline(cstr *self, FILE *stream);
 2)     bool         cstr_getdelim(cstr *self, int delim, FILE *stream);
```
`1-2)` Read a line of text from *stream* and store it in string. Line is separated by *delim*, which is *'\n'* in `1)`.
```c
        const char*  cstr_to_raw(const cstr* x);
        int          cstr_compare_raw(const char** x, const char** y);
        bool         cstr_equals_raw(const char** x, const char** y);
        uint32_t     cstr_hash_raw(const char* const* spp, size_t ignored);
        int          c_strncasecmp(const char* s1, const char* s2, size_t n);
        char*        c_strnfind(const char* str, const char* needle, size_t nmax);
        char*        c_istrnfind(const char* str, const char* needle, size_t nmax);
        uint32_t     c_string_hash(const char* str);
```
Helper methods, used by other container types.

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