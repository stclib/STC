// create a structure like: std::map<std::string, std::map<std::string, std::string>>:
#include <stc/cstr.h>

// People: map cstr -> cstr. (name -> email)
#define T People, cstr, cstr, (c_keypro | c_valpro)
#define i_keydrop(p) (printf("kdrop: %s\n", cstr_str(p)), cstr_drop(p)) // override
#include <stc/hashmap.h>

// Departments: map cstr -> People. People is a map and has _clone, _drop, therefore a "class".
#define T Departments, cstr, People, (c_keypro | c_valclass)
#include <stc/hashmap.h>


void add(Departments* deps, const char* name, const char* email, const char* dep)
{
    People *people = &Departments_emplace(deps, dep, People_init()).ref->second;
    People_put(people, name, email);
}

int contains(Departments* map, const char* name)
{
    int count = 0;
    for (c_each_item(e, Departments, *map))
        if (People_contains(&e->second, name))
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

    for (c_each_kv(dep, people, Departments, map))
        for (c_each_kv(name, email, People, *people))
            printf("%s: %s - %s\n", cstr_str(dep), cstr_str(name), cstr_str(email));
    puts("");

    printf("found Nick Denton: %d\n", contains(&map, "Nick Denton"));
    printf("found Patrick Dust: %d\n", contains(&map, "Patrick Dust"));
    printf("found Dennis Kay: %d\n", contains(&map, "Dennis Kay"));
    printf("found Serena Bath: %d\n", contains(&map, "Serena Bath"));
    puts("Done");

    Departments_drop(&map);
}
