// Create a stack and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydrop(p) (printf("drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted type
#define i_val Map
#define i_valdrop(p) (printf("drop Arc:\n"), Map_drop(p))
// no need for atomic ref. count in single thread:
// no compare function available for csmap:
#define i_opt c_no_atomic|c_no_cmp
#include <stc/carc.h>

#define i_type Stack
#define i_val_arcbox Arc // define i_val_bind for carc/cbox value (not i_val)
#include <stc/cstack.h>

#define i_type List
#define i_val_arcbox Arc // as above
#include <stc/clist.h>

int main()
{
    c_auto (Stack, stack)
    c_auto (List, list)
    {
        // POPULATE stack with shared pointers to Maps:
        Map *map;
        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Joey", 1990}, {"Mary", 1995}, {"Joanna", 1992}
        });
        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Rosanna", 2001}, {"Brad", 1999}, {"Jack", 1980}
        });

        // POPULATE list:
        map = List_push_back(&list, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Steve", 1979}, {"Rick", 1974}, {"Tracy", 2003}
        });

        // Share two Maps from the stack with the list using emplace (clone the carc):
        List_push_back(&list, Arc_clone(stack.data[0]));
        List_push_back(&list, Arc_clone(stack.data[1]));

        // Clone (deep copy) a Map from the stack to the list
        // List will contain two shared and two unshared maps.
        map = List_push_back(&list, Arc_from(Map_clone(*stack.data[1].get)))->get;

        // Add one more element to the cloned map:
        Map_emplace_or_assign(map, "CLONED", 2021);

        // Add one more element to the shared map:
        Map_emplace_or_assign(stack.data[1].get, "SHARED", 2021);


        puts("STACKS");
        c_foreach (i, Stack, stack) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", cstr_str(&_.name), _.year);
            puts("");
        }
        puts("LIST");
        c_foreach (i, List, list) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", cstr_str(&_.name), _.year);
            puts("");
        }
    }
}
