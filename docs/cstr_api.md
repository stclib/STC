# String type cstr_t

This describes the API of string type **cstr_t**.

## Types

| Type name         | Type definition                  | Used to represent...       |
|:------------------|:---------------------------------|:---------------------------|
| `cstr_t`          | `struct { const char *str; }`    | The string type            |
| `cstr_value_t`    | `char`                           | The string element type    |
| `cstr_iter_t`     | `struct { cstr_value_t *val; }`  | cstr_t iterator            |

## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `cstr_inits`              | `{...}`          |
|  `cstr_npos`               | `-1ull`          |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cstr.h"
```
## Methods

### Construction

The interface for cstr_t:
```c
cstr_t        cstr_init(void);                           (1)
cstr_t        cstr_with_capacity(size_t cap);            (2)
cstr_t        cstr_with_size(size_t len, char fill);     (3)
cstr_t        cstr_from(const char* str);                (4)
cstr_t        cstr_from_n(const char* str, size_t len);  (5)
cstr_t        cstr_from_fmt(const char* fmt, ...);       (6)
cstr_t        cstr_clone(cstr_t s);                      (7)
```
(1) Create an empty cstr_t, (2) with capacity `cap`. (3) Create a cstr_t by repeating the `fill` character `len` times. (4) Construct a cstr_t from a const char* str, and (5) limit the length by `len` and `strlen(str)`. (6) Construct a string from a formatted const char* `fmt` and arguments, using `printf()` formatting. (7) Construct a new string by cloning another cstr_t `s`.

### Destruction
```c
void          cstr_del(cstr_t *self);
```
Free the allocated memory used by string.

### Get string properties
```c
size_t       cstr_size(cstr_t s);
size_t       cstr_length(cstr_t s);
size_t       cstr_capacity(cstr_t s);
bool         cstr_empty(cstr_t s);
```

### Get references to front and back of a cstr_t
```c
char*        cstr_front(cstr_t* self);
char*        cstr_back(cstr_t* self);
```

### Reserve capcacity, resize, and clear
```c
size_t       cstr_reserve(cstr_t* self, size_t cap);
void         cstr_resize(cstr_t* self, size_t len, char fill);
void         cstr_clear(cstr_t* self);
```

### Assignment and transfer of ownership
```c
cstr_t*      cstr_assign(cstr_t* self, const char* str);                 (1)
cstr_t*      cstr_assign_n(cstr_t* self, const char* str, size_t len);   (2)
cstr_t*      cstr_take(cstr_t* self, cstr_t s);                          (3)
cstr_t       cstr_move(cstr_t* self);                                    (4)
```
(1) Assign `str` to `*self`, (2) assign substring `str` limited by `len` and `strlen(str)`. (3) Take the constructed or moved string `s`, i.e., no allocation takes place. (4) Explicitly move `*self` to the caller of the method; `*self` becomes an empty string after move.

### Append and insert characters
```c
cstr_t*      cstr_append(cstr_t* self, const char* str);                          (1)
cstr_t*      cstr_append_n(cstr_t* self, const char* str, size_t len);            (2)
cstr_t*      cstr_push_back(cstr_t* self, char ch);                               (3)
void         cstr_insert(cstr_t* self, size_t pos, const char* str);              (4)
void         cstr_insert_n(cstr_t* self, size_t pos, const char* str, size_t n);  (5)
```
(1) Append `str` to `*self`. (2) Append substring `str` limited by `len`. (3), Append character `ch`.
(4) Insert a string at the specified position (5), or insert string limited with n / strlen(str).

### Erase characters
```c
void         cstr_erase(cstr_t* self, size_t pos, size_t n);
void         cstr_pop_back(cstr_t* self);
```

### Replace substring
```c
void         cstr_replace(cstr_t* self, size_t pos, size_t len, const char* str);
void         cstr_replace_n(cstr_t* self, size_t pos, size_t len, const char* str, size_t n);
```

### Search for substring, case sensitive / insensitive
```c
size_t       cstr_find(cstr_t s, const char* substr);
size_t       cstr_find_n(cstr_t s, const char* substr, size_t pos, size_t nlen);
bool         cstr_contains(cstr_t s, const char* substr);
bool         cstr_begins_with(cstr_t s, const char* substr);
bool         cstr_ends_with(cstr_t s, const char* substr);

size_t       cstr_ifind_n(cstr_t s, const char* substr, size_t pos, size_t nlen);
bool         cstr_icontains(cstr_t s, const char* substr);
bool         cstr_ibegins_with(cstr_t s, const char* substr);
bool         cstr_iends_with(cstr_t s, const char* substr);
```

### Comparisons and equality
```c
bool         cstr_equals(cstr_t s, const char* str);
bool         cstr_equals_s(cstr_t s, cstr_t s2);
int          cstr_compare(const cstr_t *s1, const cstr_t *s2);

bool         cstr_iequals(cstr_t s, const char* str);
```

### Iterator methods
```c
cstr_iter_t  cstr_begin(cstr_t* self);     (1)
cstr_iter_t  cstr_end(cstr_t* self);       (2)
void         cstr_next(cstr_iter_t* it);   (3)
char*        cstr_itval(cstr_iter_t it);   (4)
```
To iterate though a string, one can use the generic `c_foreach` macro. E.g. `c_foreach (i, cstr, mystr) printf("%c", *i.val);`. This is equivalent to `for (size_t i=0; i<cstr_size(mystr); ++i) printf("%c", mystr.str[i])`.

```c
bool         cstr_getline(cstr_t *self, FILE *stream);
bool         cstr_getdelim(cstr_t *self, int delim, FILE *stream);
```

## Other string methods

### Non-members
```c
int          c_strncasecmp(const char* s1, const char* s2, size_t n);
char*        c_strnfind(const char* str, const char* needle, size_t nmax);
char*        c_istrnfind(const char* str, const char* needle, size_t nmax);
uint32_t     c_string_hash(const char* str);
```

### Helper methods
```c
const char*  cstr_to_raw(const cstr_t* x);
int          cstr_compare_raw(const char** x, const char** y);
bool         cstr_equals_raw(const char** x, const char** y);
uint32_t     cstr_hash_raw(const char* const* spp, size_t ignored);
```

Example:
```c
#include "stc/cstr.h"

int main() {
    cstr_t s1 = cstr_from("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&s1, cstr_find(&s1, "seven"), 5, "four");
    printf("%s.\n", s1.str);

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);

    cstr_t full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", full_path.str);

    c_del(cstr, &s1, &full_path);
}
// Output:
one-nine-three-seven-five.
one-two-nine-three-seven-five.
one-two-three-seven-five.
one-two-three-four-five.
append: one two three four five six seven eight
directory/filename.ext
```