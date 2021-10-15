// unordered_map<string, unordered_map<string, string>>:

#define i_key_str
#define i_val_str
#define i_tag sect
#include <stc/csmap.h>

#define i_key_str
#define i_val csmap_sect
#define i_valdel csmap_sect_del
#define i_tag conf
#include <stc/csmap.h>

void add(csmap_conf* map, const char* section, const char* entry, const char* value)
{
    csmap_sect *smap = &csmap_conf_insert(map, cstr_from(section), csmap_sect_init()).ref->second;
    csmap_sect_emplace_or_assign(smap, entry, value);
}

bool contains(csmap_conf* map, const char* section, const char* entry)
{
    csmap_conf_value_t *val = csmap_conf_get(map, section);
    return val && csmap_sect_get(&val->second, entry);
}

int main(void)
{
    c_auto (csmap_conf, map)
    {
        add(&map, "user", "name", "Joe");
        add(&map, "user", "groups", "proj1,proj3");
        add(&map, "group", "proj1", "Energy");
        add(&map, "group", "proj2", "Windy");
        add(&map, "group", "proj3", "Oil");
        add(&map, "admin", "employees", "2302");
        add(&map, "group", "proj2", "Wind"); // Update

        printf("contains: %d\n", contains(&map, "group", "employees"));
        printf("contains: %d\n", contains(&map, "admin", "name"));
        printf("contains: %d\n", contains(&map, "admin", "employees"));

        c_foreach (i, csmap_conf, map)
            c_foreach (j, csmap_sect, i.ref->second)
                printf("%s: %s - %s\n", i.ref->first.str, j.ref->first.str, j.ref->second.str);
    }
}