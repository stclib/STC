# Container cptr: Shared Ptr

This describes the API of the queue type **cptr**.

 Declaration

```c
#define using_cptr(X, Value, valueDestroy=c_default_del,
                             valueCompare=c_default_compare)

#define using_csptr(X, Value, valueDestroy=c_default_del,
                              valueCompare=c_default_compare)
```
The macro `using_cptr()` must be instantiated in the global scope. `X` is a type tag name and will
affect the names of all cptr types and methods. E.g. declaring `using_cptr(my, cvec_my);`,
`X` should be replaced by `my` in all of the following documentation.

 Types

| Type name           | Type definition                        | Used to represent...     |
|:--------------------|:---------------------------------------|:-------------------------|
| `cptr_X`            | Depends on underlying container type   | The cptr type            |
| `cptr_X_value_t`    | "                                      | The cptr element type    |
| `cptr_X_iter_t`     | "                                      | cptr iterator            |


| Type name           | Type definition                        | Used to represent...     |
|:--------------------|:---------------------------------------|:-------------------------|
| `csptr_X`           | Depends on underlying container type   | The csptr type           |
| `csptr_X_value_t`   | "                                      | The csptr element type   |
| `csptr_X_iter_t`    | "                                      | csptr iterator           |


Header file

All cptr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cptr.h"
```

 Methods

```c
cptr_X              cptr_X_init(void);
void                cptr_X_del(cptr_X* self);
void                cptr_X_reset(cptr_X* self, cptr_X_value_t* ptr);

int                 cptr_X_compare(cptr_X* x, cptr_X* y);
```

```c
csptr_X             csptr_X_from(csptr_X_value_t* ptr);
csptr_X             csptr_X_make(csptr_X_value_t val);
csptr_X             csptr_X_share(csptr_X ptr);
void                csptr_X_del(csptr_X* self);
void                csptr_X_reset(csptr_X* self, csptr_X_value_t* p);

int                 csptr_X_compare(csptr_X* x, csptr_X* y);
int                 csptr_pointer_compare(cptr_X_value_t* x, cptr_X_value_t* y);
```


 Example
```c
#include <stc/cptr.h>
#include <stc/cmap.h>
#include <stc/cstr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_from(Person* p, cstr_t name, cstr_t last) {
    p->name = name, p->last = last;
    return p;
}
void Person_del(Person* p) {
    c_del(cstr, &p->name, &p->last);
}

using_csptr(ps, Person, Person_del, c_no_compare);
using_cmap(ps, int, csptr_ps, csptr_ps_del);


int main() {
    cmap_ps map = cmap_inits;
    c_forrange (i, 20)
        c_try_emplace(&map, cmap_ps, (i * 7) % 10,
                            csptr_ps_from(Person_from(c_new(Person), cstr_from_fmt("Name %d", (i * 7) % 10),
                                                                     cstr_from_fmt("Last %d", (i * 9) % 10))));
    c_foreach (i, cmap_ps, map)
        printf(" %d: %s\n", i.val->first, i.val->second.get->name.str);
    cmap_ps_del(&map);
}
```
Output:
```
top: 81
```