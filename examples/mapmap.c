
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

static void my_str_destr(cstr_t* x) {
    printf("destroy: %s\n", x->str);
    cstr_destroy(x);
}

declare_cmap_strkey(ss, cstr_t, my_str_destr);
declare_cmap_strkey(cfg, cmap_ss, cmap_ss_destroy);

int main(void) {
    cmap_cfg config = cmap_init;
    cmap_ss init = cmap_init;
    cmap_ss_put(&cmap_cfg_insert(&config, "user", init)->value, "name", cstr_make("Joe"));
    cmap_ss_put(&cmap_cfg_insert(&config, "user", init)->value, "groups", cstr_make("proj1,proj3"));
    cmap_ss_put(&cmap_cfg_insert(&config, "group", init)->value, "proj1", cstr_make("Energy"));
    cmap_ss_put(&cmap_cfg_insert(&config, "group", init)->value, "proj2", cstr_make("Windy"));
    cmap_ss_put(&cmap_cfg_insert(&config, "group", init)->value, "proj3", cstr_make("Oil"));
    cmap_ss_put(&cmap_cfg_insert(&config, "admin", init)->value, "employees", cstr_make("2302"));

    cmap_ss_put(&cmap_cfg_insert(&config, "group", init)->value, "proj2", cstr_make("Wind")); // Update
    puts("");

    c_foreach (i, cmap_cfg, config)
        c_foreach (j, cmap_ss, i.item->value)
            printf("%s: %s - %s (%u)\n", i.item->key.str, j.item->key.str, j.item->value.str, i.item->value.bucket_count);

    puts("");
    cmap_cfg_destroy(&config);
}