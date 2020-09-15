// xx3.c

#include <stc/cmap.h>
#include <stc/cstr.h>

c_cmap_strkey(si, int);
c_cmap_strkey(ss, cstr_t, cstr_destroy);

int main ()
{
    cmap_si mymap = cmap_ini;
    cmap_si_put(&mymap, "Mars", 3000);
    cmap_si_put(&mymap, "Saturn", 60000);
    cmap_si_put(&mymap, "Jupiter", 70000);

    // ...
    cmap_si_put(&mymap, "Mars", 3396);
    cmap_si_emplace(&mymap, "Saturn", 0).first->value += 272;
    cmap_si_put(&mymap, "Jupiter", cmap_si_emplace(&mymap, "Saturn", 0).first->value + 9638);
    cmap_si_emplace(&mymap, "Sun", 0).first->value += 1000;
    cmap_si_emplace(&mymap, "Sun", 0).first->value += 1000;

    c_foreach (x, cmap_si, mymap) {
        printf("%s: %d\n", x.get->key.str, x.get->value);
    }
    cmap_si_destroy(&mymap);

    puts("------------------------");

    // Create an unordered_map of three strings (that map to strings)
    cmap_ss u = cmap_ini;
    cmap_ss_put(&u, "RED", cstr("#FF0000"));
    cmap_ss_put(&u, "GREEN", cstr("#00FF00"));
    cmap_ss_put(&u, "BLUE", cstr("#0000FF"));
 
    // Iterate and print keys and values of unordered_map
    c_foreach (n, cmap_ss, u) {
        printf("%s: %s\n", n.get->key.str, n.get->value.str);
    }

    // Add two new entries to the unordered_map
    cmap_ss_put(&u, "BLACK", cstr("#000000"));
    cmap_ss_put(&u, "WHITE", cstr("#FFFFFF"));
 
    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cmap_ss_find(&u, "RED")->value.str);
    printf("The HEX of color BLACK is:[%s]\n", cmap_ss_find(&u, "BLACK")->value.str);
    cmap_ss_destroy(&u);
    return 0;
}