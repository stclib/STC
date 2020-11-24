# Introduction

This describes the API of the **cstr_t** API

## Types

| cstr              | Type definition                        | Used to represent...                 |
|:------------------|:---------------------------------------|:-------------------------------------|
| `cstr_t`          | `struct { const char *str; }`          | The string type                      |
| `cstr_value_t`    | `char`                                 | The string element type              |
| `cstr_iter_t`     | `struct { char *val; }`                | String iterator                      |

## Constants

| cstr constant name         | Numerical values |
|:---------------------------|:-----------------|
|  `cstr_npos`               | `-1ULL`          |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file:

```c
#include "stc/cstr.h"
```

## Construction

The interfaces to create a cstr_t object:
```c
cstr_t cstr_init( void );
cstr_t cstr_with_capacity( size_t cap );
cstr_t cstr_with_size( size_t len, char fill );
cstr_t cstr_from( const char* str );
cstr_t cstr_from_n( const char* str, size_t len );
cstr_t cstr_from_fmt( const char* fmt, ... );
```

## Destruction

```c
void cstr_del( cstr_t *self );
```

## Interface

```c
size_t       cstr_size( cstr_t s );
size_t       cstr_length( cstr_t s );
size_t       cstr_capacity( cstr_t s );
bool         cstr_empty( cstr_t s );
```

```c
size_t       cstr_reserve( cstr_t* self, size_t cap );
void         cstr_resize( cstr_t* self, size_t len, char fill );
void         cstr_clear( cstr_t* self );
```

```c
cstr_t*      cstr_assign( cstr_t* self, const char* str );
cstr_t*      cstr_assign_n( cstr_t* self, const char* str, size_t len );
cstr_t*      cstr_take( cstr_t* self, cstr_t s );
cstr_t       cstr_clone( cstr_t s );
cstr_t       cstr_move( cstr_t* self );
```

```c
cstr_t*      cstr_append( cstr_t* self, const char* str );
cstr_t*      cstr_append_n( cstr_t* self, const char* str, size_t len );
cstr_t*      cstr_push_back( cstr_t* self, char value );
```

```c
void         cstr_insert( cstr_t* self, size_t pos, const char* str );
void         cstr_insert_n( cstr_t* self, size_t pos, const char* str, size_t n );
```

```c
void         cstr_erase( cstr_t* self, size_t pos, size_t n );
void         cstr_pop_back( cstr_t* self );
```

```c
void         cstr_replace( cstr_t* self, size_t pos, size_t len, const char* str );
void         cstr_replace_n( cstr_t* self, size_t pos, size_t len, const char* str, size_t n );
```

```c
bool         cstr_equals( cstr_t s1, const char* str );
bool         cstr_equals_caseins( cstr_t s1, const char* str );
bool         cstr_equals_s( cstr_t s1, cstr_t s2 );
int          cstr_compare( const cstr_t *s1, const cstr_t *s2 );
int          cstr_casecmp( const cstr_t *s1, const cstr_t *s2 );
size_t       cstr_find( cstr_t s, const char* needle );
size_t       cstr_find_n( cstr_t s, const char* needle, size_t pos, size_t nlen );
```

```c
char*        cstr_front( cstr_t* self );
char*        cstr_back( cstr_t* self );
cstr_iter_t  cstr_begin( cstr_t* self );
cstr_iter_t  cstr_end( cstr_t* self );
void         cstr_next(cstr_iter_t* it) { ++it->val; }
char*        cstr_itval(cstr_iter_t it) {return it.val;}
```

```c
bool         cstr_contains( cstr_t s, const char* needle );
bool         cstr_begins_with( cstr_t s, const char* needle );
bool         cstr_ends_with( cstr_t s, const char* needle );
```

```c
bool         cstr_getline(cstr_t *self, FILE *stream)
bool         cstr_getdelim( cstr_t *self, int delim, FILE *stream );
```

## Non-members

```c
char*        c_strnfind( const char* s, const char* needle, size_t nlen );
int          c_strcasecmp( const char* s1, const char* s2 );
uint32_t     c_string_hash( const char* str );
```

## Helper functions

```c
const char*  cstr_to_raw( const cstr_t* x );
int          cstr_compare_raw( const char** x, const char** y );
bool         cstr_equals_raw( const char** x, const char** y );
uint32_t     cstr_hash_raw( const char* const* spp, size_t ignored );
```
