// xx3.c

#include <stc/chash.h>
#include <stc/cstring.h>

declare_CMap_str(si, int);
declare_CMap_str(ss, CStr, cstr_destroy);

int main ()
{
    CMap_si mymap = cmap_init;
    cmap_si_put(&mymap, "Mars", 3000);
    cmap_si_put(&mymap, "Saturn", 60000);
    cmap_si_put(&mymap, "Jupiter", 70000);

    // ...
    cmap_si_put(&mymap, "Mars", 3396);
    cmap_si_at(&mymap, "Saturn", 0)->value += 272;
    cmap_si_put(&mymap, "Jupiter", cmap_si_at(&mymap, "Saturn", 0)->value + 9638);
    cmap_si_at(&mymap, "Sun", 0)->value += 1000;
    cmap_si_at(&mymap, "Sun", 0)->value += 1000;

    c_foreach (x, cmap_si, mymap) {
        printf("%s: %d\n", x.item->key.str, x.item->value);
    }
    cmap_si_destroy(&mymap);

    puts("------------------------");

    // Create an unordered_map of three strings (that map to strings)
    CMap_ss u = cmap_init;
    cmap_ss_put(&u, "RED", cstr_make("#FF0000"));
    cmap_ss_put(&u, "GREEN", cstr_make("#00FF00"));
    cmap_ss_put(&u, "BLUE", cstr_make("#0000FF"));
 
    // Iterate and print keys and values of unordered_map
    c_foreach (n, cmap_ss, u) {
        printf("%s: %s\n", n.item->key.str, n.item->value.str);
    }

    // Add two new entries to the unordered_map
    cmap_ss_put(&u, "BLACK", cstr_make("#000000"));
    cmap_ss_put(&u, "WHITE", cstr_make("#FFFFFF"));
 
    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cmap_ss_get(&u, "RED")->value.str);
    printf("The HEX of color BLACK is:[%s]\n", cmap_ss_get(&u, "BLACK")->value.str);
    cmap_ss_destroy(&u);
    return 0;
}