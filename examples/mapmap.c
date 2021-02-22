
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

using_cmap_str();
using_cmap_strkey(cfg, cmap_str, cmap_str_del, c_no_clone);

int main(void) {
    cmap_cfg config = cmap_cfg_init();
    cmap_str init = cmap_str_init();
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("user"), init).first->second, "name", "Joe");
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("user"), init).first->second, "groups", "proj1,proj3");
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("group"), init).first->second, "proj1", "Energy");
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("group"), init).first->second, "proj2", "Windy");
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("group"), init).first->second, "proj3", "Oil");
    cmap_str_emplace(&cmap_cfg_insert(&config, cstr_from("admin"), init).first->second, "employees", "2302");

    cmap_str_emplace_or_assign(&cmap_cfg_insert(&config, cstr_from("group"), init).first->second, "proj2", "Wind"); // Update

    c_foreach (i, cmap_cfg, config)
        c_foreach (j, cmap_str, i.ref->second)
            printf("%s: %s - %s (%u)\n", i.ref->first.str, j.ref->first.str, j.ref->second.str, i.ref->second.bucket_count);

    cmap_cfg_del(&config);
}