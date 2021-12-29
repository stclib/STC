// unordered_map<string, unordered_map<string, string>>:

#define i_type People
#define i_key_str
#define i_val_str
#define i_keydrop(p) (printf("kdrop: %s\n", (p)->str), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Departments
#define i_key_str
#define i_val_bind People
#include <stc/csmap.h>

#define i_type Stack
#define i_val_bind People_value
#define i_opt c_no_cmp
//#define i_from People_value_clone
//#define i_drop People_value_drop
#include <stc/cstack.h>

void add(Departments* deps, const char* name, const char* email, const char* dep)
{
    People *people = &Departments_insert(deps, cstr_from(dep), People_init()).ref->second;
    People_emplace_or_assign(people, name, email);
}

Stack contains(Departments* map, const char* name)
{
    Stack stk = Stack_init();
    const People_value* v;
    c_foreach (i, Departments, *map)
        if ((v = People_get(&i.ref->second, name))) {
            Stack_push(&stk, People_value_clone(*v));
        }
    return stk;
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
                printf("%s: %s - %s\n", i.ref->first.str, _.name.str, _.email.str);
        puts("");

        c_auto (Stack, s) printf("found: %zu\n", Stack_size(s = contains(&map, "Nick Denton")));
        c_auto (Stack, s) printf("found: %zu\n", Stack_size(s = contains(&map, "Patrick Dust")));
        c_auto (Stack, s) printf("found: %zu\n", Stack_size(s = contains(&map, "Dennis Kay")));
        c_auto (Stack, s) printf("found: %zu\n", Stack_size(s = contains(&map, "Serena Bath")));
        puts("Done");
    }
}
