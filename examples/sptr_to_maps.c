// Create a stack and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydel(p) (printf("del name: %s\n", (p)->str), cstr_del(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted type
#define i_val Map
#define i_del(p) (printf("del Arc:\n"), Map_del(p))
// no need for atomic ref. count in single thread:
#define i_opt c_no_atomic 
#include <stc/csptr.h>

#define i_type Stack
#define i_val_ref Arc // define i_val_ref for csptr/cbox value (not i_val)
#include <stc/cstack.h>

#define i_type List
#define i_val_ref Arc // as above
#include <stc/clist.h>

int main()
{
    c_auto (Stack, stack)
    c_auto (List, list)
    {
        // POPULATE stack with shared pointers to Maps:
        Map *map;
        map = Stack_push(&stack, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Joey", 1990}, {"Mary", 1995}, {"Joanna", 1992}
        });
        map = Stack_push(&stack, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Rosanna", 2001}, {"Brad", 1999}, {"Jack", 1980}
        });

        // POPULATE list:
        map = List_push_back(&list, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Steve", 1979}, {"Rick", 1974}, {"Tracy", 2003}
        });
        
        // Share two Maps from the stack with the list using emplace (clone the csptr):
        List_emplace_back(&list, stack.data[0]);
        List_emplace_back(&list, stack.data[1]);
        
        // Clone (deep copy) a Map from the stack to the list
        // List will contain two shared and two unshared maps.
        map = List_push_back(&list, Arc_new(Map_clone(*stack.data[1].get)))->get;
        
        // Add one more element to the cloned map:
        Map_emplace_or_assign(map, "CLONED", 2021);

        // Add one more element to the shared map:
        Map_emplace_or_assign(stack.data[1].get, "SHARED", 2021);


        puts("STACKS");
        c_foreach (i, Stack, stack) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", _.name.str, _.year);
            puts("");
        }
        puts("LIST");
        c_foreach (i, List, list) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", _.name.str, _.year);
            puts("");
        }
    }
}
