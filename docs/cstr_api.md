# STC [cstr](../include/stc/cstr.h): String
![String](pics/string.jpg)

A **cstr** object represent sequences of characters. It supports an interface similar to that of a standard container of bytes, but adding features specifically designed to operate with strings of single-byte characters, terminated by the null character.

See the c++ class [std::basic_string](https://en.cppreference.com/w/cpp/string/basic_string) for a functional description.

## Header file

All cstr definitions and prototypes are available by including a single header file.

```c
#include <stc/cstr.h>
```
## Methods

```c
cstr         cstr_init(void);                                         // constructor; same as cstr_null.
cstr         cstr_lit(const char literal_only[]);                     // cstr from literal; no strlen().
cstr         cstr_from(const char* str);                              // constructor using strlen()
cstr         cstr_from_n(const char* str, size_t n);                  // constructor with specified length
cstr         cstr_with_capacity(size_t cap);
cstr         cstr_with_size(size_t len, char fill);                   // repeat fill len times
cstr         cstr_from_fmt(const char* fmt, ...);                     // printf() formatting
cstr         cstr_clone(cstr s);

cstr*        cstr_take(cstr* self, cstr s);                           // take the constructed or moved string
cstr         cstr_move(cstr* self);                                   // move string to caller, leave empty string
void         cstr_del(cstr *self);                                    // destructor

const char*  cstr_str(const cstr* self);                              // self->str
char*        cstr_data(cstr* self);                                   // self->str
size_t       cstr_size(cstr s);
size_t       cstr_length(cstr s);
size_t       cstr_capacity(cstr s);
bool         cstr_empty(cstr s);

size_t       cstr_reserve(cstr* self, size_t capacity);
void         cstr_resize(cstr* self, size_t len, char fill);
void         cstr_clear(cstr* self);

cstr*        cstr_assign(cstr* self, const char* str);
cstr*        cstr_assign_n(cstr* self, const char* str, size_t n);    // assign n first chars of str
cstr*        cstr_assign_fmt(cstr* self, const char* fmt, ...);       // printf() formatting
cstr*        cstr_copy(cstr* self, cstr s);                           // cstr_take(self, cstr_clone(s))

cstr*        cstr_append(cstr* self, const char* str);
cstr*        cstr_append_s(cstr* self, cstr s);
cstr*        cstr_append_n(cstr* self, const char* str, size_t n);

void         cstr_insert(cstr* self, size_t pos, const char* str);
void         cstr_insert_s(cstr* self, size_t pos, cstr s);
void         cstr_insert_n(cstr* self, size_t pos, const char* str, size_t n);

void         cstr_replace(cstr* self, size_t pos, size_t len, const char* str);
void         cstr_replace_s(cstr* self, size_t pos, size_t len, cstr s);
void         cstr_replace_n(cstr* self, size_t pos, size_t len, const char* str, size_t n);
void         cstr_replace_all(cstr* self, const char* find, const char* replace);

void         cstr_erase(cstr* self, size_t pos);
void         cstr_erase_n(cstr* self, size_t pos, size_t n);

bool         cstr_equalto(cstr s, const char* str);
bool         cstr_equalto_s(cstr s, cstr s2);
size_t       cstr_find(cstr s, const char* needle);
size_t       cstr_find_n(cstr s, const char* needle, size_t pos, size_t nmax);
bool         cstr_contains(cstr s, const char* needle);
bool         cstr_starts_with(cstr s, const char* str);
bool         cstr_ends_with(cstr s, const char* str);

bool         cstr_iequalto(cstr s, const char* str);                   // prefix i = case-insensitive
size_t       cstr_ifind_n(cstr s, const char* needle, size_t pos, size_t nmax);
bool         cstr_icontains(cstr s, const char* needle);
bool         cstr_istarts_with(cstr s, const char* str);
bool         cstr_iends_with(cstr s, const char* str);

void         cstr_push_back(cstr* self, char ch);
void         cstr_pop_back(cstr* self);
char*        cstr_front(cstr* self);
char*        cstr_back(cstr* self);

cstr_iter_t  cstr_begin(cstr* self);
cstr_iter_t  cstr_end(cstr* self);
void         cstr_next(cstr_iter_t* it);

bool         cstr_getline(cstr *self, FILE *stream);                  // cstr_getdelim(self, '\n', stream)
bool         cstr_getdelim(cstr *self, int delim, FILE *stream);      // does not append delim to result
```

Note that all methods with arguments `(..., const char* str, size_t n)`, `n` must be within the range of `str` length.

#### Helper methods:
```c
int          cstr_compare(const cstr *s1, const cstr *s2);
bool         cstr_equals(const cstr *s1, const cstr *s2);
bool         cstr_hash(const cstr *s, ...);

int          c_rawstr_compare(const char** x, const char** y);
bool         c_rawstr_equals(const char** x, const char** y);
uint64_t     c_rawstr_hash(const char* const* x, ...);

int          c_strncasecmp(const char* str1, const char* str2, size_t n);
char*        c_strnstrn(const char* str, const char* needle, size_t slen, size_t nlen);
char*        c_strncasestrn(const char* str, const char* needle, size_t slen, size_t nlen);
```

## Types

| Type name         | Type definition                  | Used to represent...     |
|:------------------|:---------------------------------|:-------------------------|
| `cstr`            | `struct { char *str; }`          | The string type          |
| `cstr_value_t`    | `char`                           | The string element type  |
| `cstr_iter_t`     | `struct { cstr_value_t *ref; }`  | cstr iterator            |

## Constants and macros

| Name              | Value             |
|:------------------|:------------------|
|  `cstr_npos`      | `INTPTR_MAX`      |
|  `cstr_null`      | cstr null value   |

## Example
```c
#include <stc/cstr.h>

int main() {
    cstr s0 = cstr_lit("Initialization without using strlen().");
    printf("%s\nLength: %zu\n\n", s0.str, cstr_size(s0));

    cstr s1 = cstr_from("one-nine-three-seven-five.");
    printf("%s\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s\n", s1.str);

    cstr_erase_n(&s1, 7, 5); // -nine
    printf("%s\n", s1.str);

    cstr_replace(&s1, cstr_find(s1, "seven"), 5, "four");
    printf("%s\n", s1.str);

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);

    cstr full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", full_path.str);

    c_del(cstr, &s0, &s1, &full_path);
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
