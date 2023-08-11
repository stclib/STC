// Create a stack and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_implement
#include <stc/cstr.h>
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydrop(p) (printf("drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted type
#define i_key Map
#define i_keydrop(p) (printf("drop Arc:\n"), Map_drop(p))
// no need for atomic ref. count in single thread:
#define i_opt c_no_atomic
#include <stc/carc.h>

#define i_type Stack
#define i_keyboxed Arc // use i_keyboxed for carc/cbox key
#include <stc/cvec.h>

#define i_type List
#define i_keyboxed Arc // as above
#include <stc/clist.h>

int main(void)
{
    Stack stack = {0};
    List list = {0};
    c_defer(
        Stack_drop(&stack),
        List_drop(&list)
    ){
        // POPULATE stack with shared pointers to Maps:
        Map *map;
        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        Map_emplace(map, "Joey", 1990);
        Map_emplace(map, "Mary", 1995);
        Map_emplace(map, "Joanna", 1992);

        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        Map_emplace(map, "Rosanna", 2001);
        Map_emplace(map, "Brad", 1999);
        Map_emplace(map, "Jack", 1980);

        // POPULATE list:
        map = List_push_back(&list, Arc_from(Map_init()))->get;
        Map_emplace(map, "Steve", 1979);
        Map_emplace(map, "Rick", 1974);
        Map_emplace(map, "Tracy", 2003);

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
                printf(" %s:%d", cstr_str(_.name), *_.year);
            puts("");
        }

        puts("LIST");
        c_foreach (i, List, list) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", cstr_str(_.name), *_.year);
            puts("");
        }
    }
}
