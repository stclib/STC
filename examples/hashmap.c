// https://doc.rust-lang.org/rust-by-example/std/hash.html

#define i_key_str
#define i_val_str
#include <stdio.h>
#include <stc/cmap.h>

const char* call(const char* number) {
    if (!strcmp(number, "798-1364"))
        return "We're sorry, the call cannot be completed as dialed." 
               " Please hang up and try again.";
    else if (!strcmp(number, "645-7689")) 
        return "Hello, this is Mr. Awesome's Pizza. My name is Fred."
               " What can I get for you today?";
    else
        return "Hi! Who is this again?";
}

int main(void) {
    c_auto (cmap_str, contacts)
    {
        cmap_str_emplace(&contacts, "Daniel", "798-1364");
        cmap_str_emplace(&contacts, "Ashley", "645-7689");
        cmap_str_emplace(&contacts, "Katie", "435-8291");
        cmap_str_emplace(&contacts, "Robert", "956-1745");

        const cmap_str_value* v;
        if ((v = cmap_str_get(&contacts, "Daniel")))
            printf("Calling Daniel: %s\n", call(cstr_str(&v->second)));
        else
            printf("Don't have Daniel's number.");

        cmap_str_emplace(&contacts, "Daniel", "164-6743");

        if ((v = cmap_str_get(&contacts, "Ashley")))
            printf("Calling Ashley: %s\n", call(cstr_str(&v->second)));
        else
            printf("Don't have Ashley's number.");

        cmap_str_erase(&contacts, "Ashley");

        puts("");
        c_forpair (contact, number, cmap_str, contacts) {
            printf("Calling %s: %s\n", cstr_str(&_.contact), call(cstr_str(&_.number)));
        }
        puts("");
    }
}
