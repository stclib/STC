
#include <stdio.h>
#include <stc/cstr.h>

// unordered_map<string, unordered_map<string, string>>:

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

#define i_tag cfg
#define i_key_str
#define i_val cmap_str
#define i_valdel cmap_str_del
#include <stc/cmap.h>

int main(void)
{
    c_forauto (cmap_cfg, cfg)
    {
        cmap_str init = cmap_str_init();
        cmap_cfg_insert(&cfg, cstr_from("user"), init);
        cmap_cfg_insert(&cfg, cstr_from("group"), init);
        cmap_cfg_insert(&cfg, cstr_from("admin"), init);

        cmap_str_emplace(cmap_cfg_at(&cfg, "user"), "name", "Joe");
        cmap_str_emplace(cmap_cfg_at(&cfg, "user"), "groups", "proj1,proj3");
        cmap_str_emplace(cmap_cfg_at(&cfg, "group"), "proj1", "Energy");
        cmap_str_emplace(cmap_cfg_at(&cfg, "group"), "proj2", "Windy");
        cmap_str_emplace(cmap_cfg_at(&cfg, "group"), "proj3", "Oil");
        cmap_str_emplace(cmap_cfg_at(&cfg, "admin"), "employees", "2302");

        cmap_str_emplace_or_assign(cmap_cfg_at(&cfg, "group"), "proj2", "Wind"); // Update

        c_foreach (i, cmap_cfg, cfg)
            c_foreach (j, cmap_str, i.ref->second)
                printf("%s: %s - %s (%u)\n", i.ref->first.str, j.ref->first.str, j.ref->second.str, i.ref->second.bucket_count);
    }
}