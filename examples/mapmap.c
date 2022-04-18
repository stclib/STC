// unordered_map<string, unordered_map<string, string>>:

#define i_type People
#define i_key_str
#define i_val_str
#define i_keydrop(p) (printf("kdrop: %s\n", cstr_str(p)), cstr_drop(p)) // override
#include <stc/csmap.h>

#define i_type Departments
#define i_key_str
#define i_val_bind People
// Shorthand for:
//#define i_val People
//#define i_cmp People_cmp
//#define i_valfrom People_clone
//#define i_valdrop People_drop
#include <stc/csmap.h>

#define i_type Stack
#define i_val_bind People_value
// Shorthand for:
//#define i_val People_value
//#define i_cmp People_value_cmp
//#define i_valfrom People_value_clone
//#define i_valdrop People_value_drop
#include <stc/cvec.h>

void add(Departments* deps, const char* name, const char* email, const char* dep)
{
    People *people = &Departments_insert(deps, cstr_from(dep), People_init()).ref->second;
    People_emplace_or_assign(people, name, email);
}

int contains(Departments* map, const char* name)
{
    int count = 0;
    c_foreach (i, Departments, *map)
        if (People_contains(&i.ref->second, name))
            ++count;
    return count;
}

int main(void)
{
    c_auto (Departments, map)
    {
        add(&map, "Anna Kendro", "Anna@myplace.com", "Support");
        add(&map, "Terry Dane", "Terry@myplace.com", "Development");
        add(&map, "Kik Winston", "Kik@myplace.com", "Finance");
        add(&map, "Nancy Drew", "Nancy@live.com", "Development");
        add(&map, "Nick Denton", "Nick@myplace.com", "Finance");
        add(&map, "Stan Whiteword", "Stan@myplace.com", "Marketing");
        add(&map, "Serena Bath", "Serena@myplace.com", "Support");
        add(&map, "Patrick Dust", "Patrick@myplace.com", "Finance");
        add(&map, "Red Winger", "Red@gmail.com", "Marketing");
        add(&map, "Nick Denton", "Nick@yahoo.com", "Support");
        add(&map, "Colin Turth", "Colin@myplace.com", "Support");
        add(&map, "Dennis Kay", "Dennis@mail.com", "Marketing");
        add(&map, "Anne Dickens", "Anne@myplace.com", "Development");

        c_foreach (i, Departments, map)
            c_forpair (name, email, People, i.ref->second)
                printf("%s: %s - %s\n", cstr_str(&i.ref->first), cstr_str(&_.name), cstr_str(&_.email));
        puts("");

        printf("found: %d\n", contains(&map, "Nick Denton"));
        printf("found: %d\n", contains(&map, "Patrick Dust"));
        printf("found: %d\n", contains(&map, "Dennis Kay"));
        printf("found: %d\n", contains(&map, "Serena Bath"));
        puts("Done");
    }
}
