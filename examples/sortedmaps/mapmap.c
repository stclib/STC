// create a structure like: std::map<std::string, std::map<std::string, std::string>>:
#include "stc/cstr.h"

// People: std::map<std::string, std::string>
#define i_type People
#define i_keypro cstr // name
#define i_valpro cstr // email
#define i_keydrop(p) (printf("kdrop: %s\n", cstr_str(p)), cstr_drop(p)) // override
#include "stc/hmap.h"

// Departments: std::map<std::string, People>
#define i_type Departments
#define i_keypro cstr // dep. name
#define i_valclass People
#include "stc/hmap.h"


void add(Departments* deps, const char* name, const char* email, const char* dep)
{
    People *people = &Departments_emplace(deps, dep, People_init()).ref->second;
    People_emplace_or_assign(people, name, email);
}

int contains(Departments* map, const char* name)
{
    int count = 0;
    for (c_each(i, Departments, *map))
        if (People_contains(&i.ref->second, name))
            ++count;
    return count;
}

int main(void)
{
    Departments map = {0};

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

    for (c_each(i, Departments, map))
        for (c_each_kv(name, email, People, i.ref->second))
            printf("%s: %s - %s\n", cstr_str(&i.ref->first), cstr_str(name), cstr_str(email));
    puts("");

    printf("found Nick Denton: %d\n", contains(&map, "Nick Denton"));
    printf("found Patrick Dust: %d\n", contains(&map, "Patrick Dust"));
    printf("found Dennis Kay: %d\n", contains(&map, "Dennis Kay"));
    printf("found Serena Bath: %d\n", contains(&map, "Serena Bath"));
    puts("Done");

    Departments_drop(&map);
}
