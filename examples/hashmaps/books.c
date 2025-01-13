// https://doc.rust-lang.org/std/collections/struct.HashMap.html
#include "stc/cstr.h"
#define i_keypro cstr
#define i_valpro cstr
#include "stc/hmap.h"

// Type inference lets us omit an explicit type signature (which
// would be `HashMap<String, String>` in this example).
int main(void)
{
    hmap_cstr book_reviews = {0};

    // Review some books.
    hmap_cstr_emplace(&book_reviews,
        "Adventures of Huckleberry Finn",
        "My favorite book."
    );
    hmap_cstr_emplace(&book_reviews,
        "Grimms' Fairy Tales",
        "Masterpiece."
    );
    hmap_cstr_emplace(&book_reviews,
        "Pride and Prejudice",
        "Very enjoyable"
    );
    hmap_cstr_insert(&book_reviews,
        cstr_lit("The Adventures of Sherlock Holmes"),
        cstr_lit("Eye lyked it alot.")
    );

    // Check for a specific one.
    // When collections store owned values (String), they can still be
    // queried using references (&str).
    if (hmap_cstr_contains(&book_reviews, "Les Misérables")) {
        printf("We've got %d reviews, but Les Misérables ain't one.",
                    (int)hmap_cstr_size(&book_reviews));
    }

    // oops, this review has a lot of spelling mistakes, let's delete it.
    hmap_cstr_erase(&book_reviews, "The Adventures of Sherlock Holmes");

    // Look up the values associated with some keys.
    const char* to_find[] = {"Pride and Prejudice", "Alice's Adventure in Wonderland"};
    for (c_range(i, c_arraylen(to_find))) {
        const hmap_cstr_value* b = hmap_cstr_get(&book_reviews, to_find[i]);
        if (b)
            printf("%s: %s\n", cstr_str(&b->first), cstr_str(&b->second));
        else
            printf("%s is unreviewed.\n", to_find[i]);
    }

    // Look up the value for a key (will panic if the key is not found).
    printf("Review for Jane: %s\n", cstr_str(hmap_cstr_at(&book_reviews, "Pride and Prejudice")));

    // Iterate over everything.
    for (c_each_kv(book, review, hmap_cstr, book_reviews)) {
        printf("%s: \"%s\"\n", cstr_str(book), cstr_str(review));
    }

    hmap_cstr_drop(&book_reviews);
}
