#include <stc/c11/serialize.h>

// header ------------------------

#define serde_struct_Employee \
    (id,, int), \
    (name, cstr,), \
//
#define serde_struct_Department \
    (name,, const char*), \
    (project_ids, IntList,), \
    (leader, Employee,, CSERDE_PTR), \
    (employees, Employees,), \
//
/* Employee */
cserde_define_struct(Employee);
void Employee_drop(Employee* e);

/* IntList */
#define T IntList, int, (c_use_serde)
#include <stc/stack.h>

/* Employees */
#define T Employees, Employee, (c_class_key| c_no_clone| c_use_serde)
#include <stc/stack.h>

/* Department */
cserde_define_struct(Department);
void Department_drop(Department* e);

/* Departments */
#define T Departments, Department, (c_class_key| c_no_clone| c_use_serde)
#include <stc/stack.h>

// main ------------------------

int main(void) {
    Departments departments = c_make(Departments, {{
        .name = "Engineering\tDep.\n",
        .project_ids = c_make(IntList, {32, 32, 65, 78, 23, 54}),
        .employees = c_make(Employees, {
            {.id = 1, .name = cstr_from("Alice")},
            {.id = 2, .name = cstr_from("Bob")},
        })
    },{
        .name = "Management",
        .project_ids = c_make(IntList, {282, /*322, 54, 766*/}),
        .employees = c_make(Employees, {
            {.id = 3, .name = cstr_from("Charlie\nCook")},
            {.id = 4, .name = cstr_from("Monica")},
        })
    }});
    departments.data[0].leader = &departments.data[0].employees.data[1];
    departments.data[1].leader = &departments.data[1].employees.data[0];

    cserde_archive ar = {cserde_pretty, {stdout}};
    //cserde_archive ar = {cserde_pretty};
    Departments_to_json(&departments, &ar);

    if (ar.stream.data)
        printf("string:\n%s", ar.stream.data);
    puts("\n");

    Departments_drop(&departments);
    cserde_archive_drop(&ar); // close file or free data
}

// implement ------------------------

cserde_implement_struct(Employee);
cserde_implement_struct(Department);

void Employee_drop(Employee* e) {
    //printf("dropping employee: %s\n", cstr_str(&e->name));
    cstr_drop(&e->name);
}

void Department_drop(Department* e) {
    //printf("dropping department: %s\n", cstr_str(&e->name));
    //cstr_drop(&e->name);
    Employees_drop(&e->employees);
}
