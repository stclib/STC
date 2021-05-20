#include <stc/csview.h>
#include <stc/cmap.h>
#include <stdio.h>

// cmap<cstr, int> with csview as convertion key-type (raw):
using_cmap_svkey(si, int);

int main() {
    csview fox = c_lit("The quick brown fox jumps over the lazy dog.");
    printf("\"%s\", length=%zu\n", fox.str, fox.size);

    c_withvar (cmap_si, frequencies)
    {
        // Non-emplace: cstr element API
        cmap_si_insert(&frequencies, cstr_lit("thousand"), 1000);
        cmap_si_insert_or_assign(&frequencies, cstr_lit("thousand"), 2000); // update; same as put()
        cmap_si_insert(&frequencies, cstr_lit("thousand"), 3000); // ignored

        // Emplace: csview element API
        const char* key = "hundred";
        cmap_si_emplace(&frequencies, c_lit("hundred"), 300); // c_lit() shorthand for csview_lit()
        cmap_si_emplace(&frequencies, csview_from_n(key, 4), 400); // insert "hund"
        cmap_si_emplace_or_assign(&frequencies, csview_from(key), 500); // update
        cmap_si_emplace(&frequencies, c_lit("hundred"), 600);      // ignored, already inserted

        // Lookup always uses "raw" type API, i.e. csview here.
        printf("at(\"hundred\"): %d\n", *cmap_si_at(&frequencies, c_lit("hundred")));

        c_foreach (i, cmap_si, frequencies)
            printf("%s: %d\n", i.ref->first.str, i.ref->second);
    }
    /* Output: */
    // "The quick brown fox jumps over the lazy dog.", length=44
    // at("hundred"): 500
    // hund: 400
    // hundred: 500
    // thousand: 2000
}