#include <stdio.h>
#include <stc/cstr.h>

// box<long>
#define T boxlong, long //, (c_use_cmp) // add if compare is needed
#include <stc/box.h>

// hashmap<cstr, boxlong>
#define T Magicmap, cstr, boxlong, (c_keypro | c_valpro)
#include <stc/hashmap.h>

int main(void)
{
    // c_make() and emplace() implicitly creates cstr from const char*
    // and a "boxed long" from long.
    Magicmap map = c_make(Magicmap, {
        {"just", 10},
        {"some", 20},
        {"random", 30},
        {"words", 40},
    });
    Magicmap_emplace(&map, "another", 100);

    printf("Lookup \"some\": %ld\n\n", *Magicmap_at(&map, "some")->get);

    for (c_each_kv(name, num, Magicmap, map)) {
        printf("%s: %ld\n", cstr_str(name), *num->get);
    }

    Magicmap_drop(&map);
}
