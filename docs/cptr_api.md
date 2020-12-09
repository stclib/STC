# Module cuptr: Smart Pointers

This describes the API of the type **cuptr** and the shared pointer type **csptr**. The **cuptr** is meant to be used like a c++ std::unique_ptr, while **csptr** is similar to c++ std::shared_ptr.

The **cuptr** type is meant to be used as elements of containers, because the pointed to elements will automatically be deleted when the container is deleted. On its own, it offers little over regular pointers. **csptr** however, is useful when you want to track and delete the last usage of a pointer. **csptr** uses atomic usage counting, via the *csptr_X_share(sp)* and *csptr_X_del(&sp)* methods.

## Declaration

```c
#define using_cuptr(X, Value, valueDestroy=c_default_del,
                              valueCompare=c_default_compare)

#define using_csptr(X, Value, valueDestroy=c_default_del,
                              valueCompare=c_default_compare)
```
The macro `using_cuptr()` must be instantiated in the global scope. `X` is a type tag name and will
affect the names of all cuptr types and methods. E.g. declaring `using_cuptr(my, cvec_my);`,
`X` should be replaced by `my` in all of the following documentation.

 Types

| Type name           | Type definition                        | Used to represent...     |
|:--------------------|:---------------------------------------|:-------------------------|
| `cuptr_X`           | Depends on underlying container type   | The cuptr type            |
| `cuptr_X_value_t`   | "                                      | The cuptr element type    |
| `cuptr_X_iter_t`    | "                                      | cuptr iterator            |


| Type name           | Type definition                        | Used to represent...     |
|:--------------------|:---------------------------------------|:-------------------------|
| `csptr_X`           | Depends on underlying container type   | The csptr type           |
| `csptr_X_value_t`   | "                                      | The csptr element type   |
| `csptr_X_iter_t`    | "                                      | csptr iterator           |


## Header file

All cptr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cptr.h"
```

## Methods

```c
cuptr_X             cuptr_X_init(void);
void                cuptr_X_del(cuptr_X* self);
void                cuptr_X_reset(cuptr_X* self, cuptr_X_value_t* ptr);

int                 cuptr_X_compare(cuptr_X* x, cuptr_X* y);
```

```c
csptr_X             csptr_X_from(csptr_X_value_t* ptr);
csptr_X             csptr_X_make(csptr_X_value_t val);
csptr_X             csptr_X_share(csptr_X ptr);
void                csptr_X_del(csptr_X* self);
void                csptr_X_reset(csptr_X* self, csptr_X_value_t* p);

int                 csptr_X_compare(csptr_X* x, csptr_X* y);
int                 csptr_pointer_compare(csptr_X_value_t* x, csptr_X_value_t* y);
```

## Example
This shows three maps with struct Person as the mapped type in different ways, map1 with Person value, map2 with bare pointer to Person, and map3 with a shared pointer to Person.
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
    printf("del: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

using_cmap(pv, int, Person, Person_del);           // mapped: Person

using_cuptr(pp, Person, Person_del, c_no_compare);
using_cmap(pp, int, cuptr_pp, cuptr_pp_del);       // mapped: Person*

using_csptr(ps, Person, Person_del, c_no_compare);
using_cmap(ps, int, csptr_ps, csptr_ps_del);       // mapped: csptr<Person>


int main() {
    cmap_pv map1 = cmap_inits;  // mapped: Person
    cmap_pv_put(&map1, 1990, (Person){cstr_from("Joe 1"), cstr_from("Average")});
    cmap_pv_put(&map1, 1985, (Person){cstr_from("John 1"), cstr_from("Smith")});
    c_foreach (i, cmap_pv, map1)
        printf("   %d: %s\n", i.val->first, i.val->second.name.str);
    cmap_pv_del(&map1);

    cmap_pp map2 = cmap_inits;  // mapped: Person*
    cmap_pp_put(&map2, 1990, Person_from(c_new(Person), cstr_from("Joe 2"), cstr_from("Average")));
    cmap_pp_put(&map2, 1985, Person_from(c_new(Person), cstr_from("John 2"), cstr_from("Smith")));
    c_foreach (i, cmap_pp, map2)
        printf("   %d: %s\n", i.val->first, i.val->second->name.str);
    cmap_pp_del(&map2); 
    
    cmap_ps map3 = cmap_inits;  // mapped: csptr<Person>
    cmap_ps_put(&map3, 1990, csptr_ps_from(Person_from(c_new(Person), cstr_from("Joe 3"), cstr_from("Average"))));
    cmap_ps_put(&map3, 1985, csptr_ps_from(Person_from(c_new(Person), cstr_from("John 3"), cstr_from("Smith"))));

    csptr_ps x = csptr_ps_share(*cmap_ps_at(&map3, 1990)); // share an item in the map
    
    c_foreach (i, cmap_ps, map3)
        printf("   %d: %s\n", i.val->first, i.val->second.get->name.str);
    cmap_ps_del(&map3); 
    
    printf("x: 1990: %s\n", x.get->name.str);
    csptr_ps_del(&x);
}
```
Output:
```
   1990: Joe 1
   1985: John 1
del: Joe 1 Average
del: John 1 Smith
   1990: Joe 2
   1985: John 2
del: Joe 2 Average
del: John 2 Smith
   1990: Joe 3
   1985: John 3
del: John 3 Smith
x: 1990: Joe 3
del: Joe 3 Average
```
