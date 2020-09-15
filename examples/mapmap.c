
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

cdef_cmap_str();
cdef_cmap_strkey(cfg, cmap_str, cmap_str_destroy);

int main(void) {
    cmap_cfg config = cmap_ini;
    cmap_str init = cmap_ini;
    cmap_str_put(&cmap_cfg_emplace(&config, "user", init).first->value, "name", "Joe");
    cmap_str_put(&cmap_cfg_emplace(&config, "user", init).first->value, "groups", "proj1,proj3");
    cmap_str_put(&cmap_cfg_emplace(&config, "group", init).first->value, "proj1", "Energy");
    cmap_str_put(&cmap_cfg_emplace(&config, "group", init).first->value, "proj2", "Windy");
    cmap_str_put(&cmap_cfg_emplace(&config, "group", init).first->value, "proj3", "Oil");
    cmap_str_put(&cmap_cfg_emplace(&config, "admin", init).first->value, "employees", "2302");

    cmap_str_put(&cmap_cfg_emplace(&config, "group", init).first->value, "proj2", "Wind"); // Update

    c_foreach (i, cmap_cfg, config)
        c_foreach (j, cmap_str, i.get->value)
            printf("%s: %s - %s (%u)\n", i.get->key.str, j.get->key.str, j.get->value.str, i.get->value.bucket_count);

    cmap_cfg_destroy(&config);
}