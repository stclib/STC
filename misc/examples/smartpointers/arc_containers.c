// Create a vec and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_implement
#include "stc/cstr.h"
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydrop(p) (printf("drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include "stc/smap.h"

#define i_type Arc // (atomic) ref. counted type
#define i_key Map
#define i_keydrop(p) (printf("drop Arc:\n"), Map_drop(p))
// no need for atomic ref. count in single thread:
#define i_opt c_no_atomic
#include "stc/arc.h"

#define i_type Vec
#define i_key_arcbox Arc // use i_key_arcbox for arc/box key
#include "stc/vec.h"

#define i_type List
#define i_key_arcbox Arc // as above
#include "stc/list.h"

int main(void)
{
    Vec vec = {0};
    List list = {0};
    c_defer(
        Vec_drop(&vec),
        List_drop(&list)
    ){
        // POPULATE vec with shared pointers to Maps:
        Map *map;
        //map = Vec_push(&vec, Arc_from(Map_init()))->get;
        map = Vec_emplace(&vec, Map_init())->get;
        Map_emplace(map, "Joey", 1990);
        Map_emplace(map, "Mary", 1995);
        Map_emplace(map, "Joanna", 1992);

        //map = Vec_push(&vec, Arc_from(Map_init()))->get;
        map = Vec_emplace(&vec, Map_init())->get;
        Map_emplace(map, "Rosanna", 2001);
        Map_emplace(map, "Brad", 1999);
        Map_emplace(map, "Jack", 1980);

        // POPULATE list:
        //map = List_push_back(&list, Arc_from(Map_init()))->get;
        map = List_emplace_back(&list, Map_init())->get;
        Map_emplace(map, "Steve", 1979);
        Map_emplace(map, "Rick", 1974);
        Map_emplace(map, "Tracy", 2003);

        // Share two Maps from the vec with the list using emplace (clone the arc):
        List_push_back(&list, Arc_clone(vec.data[0]));
        List_push_back(&list, Arc_clone(vec.data[1]));

        // Clone (deep copy) a Map from the vec to the list
        // List will contain two shared and two unshared maps.
        map = List_push_back(&list, Arc_from(Map_clone(*vec.data[1].get)))->get;

        // Add one more element to the cloned map:
        Map_emplace_or_assign(map, "CLONED", 2021);

        // Add one more element to the shared map:
        Map_emplace_or_assign(vec.data[1].get, "SHARED", 2021);

        puts("VEC");
        c_foreach (i, Vec, vec) {
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
