
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

// unordered_map<string, unordered_map<string, string>>: 

using_cmap_str();
using_cmap_strkey(cfg, cmap_str, cmap_str_del, c_no_clone);

int main(void) {
    c_forvar (cmap_cfg cfg = cmap_cfg_init(), cmap_cfg_del(&cfg)) {
        cmap_str init = cmap_str_init();
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("user"), init).ref->second, "name", "Joe");
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("user"), init).ref->second, "groups", "proj1,proj3");
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("group"), init).ref->second, "proj1", "Energy");
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("group"), init).ref->second, "proj2", "Windy");
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("group"), init).ref->second, "proj3", "Oil");
        cmap_str_emplace(&cmap_cfg_insert(&cfg, cstr_from("admin"), init).ref->second, "employees", "2302");

        cmap_str_emplace_or_assign(&cmap_cfg_insert(&cfg, cstr_from("group"), init).ref->second, "proj2", "Wind"); // Update

        c_foreach (i, cmap_cfg, cfg)
            c_foreach (j, cmap_str, i.ref->second)
                printf("%s: %s - %s (%u)\n", i.ref->first.str, j.ref->first.str, j.ref->second.str, i.ref->second.bucket_count);
    }
}