// https://doc.rust-lang.org/std/collections/struct.HashMap.html
#include <stc/cstr.h>
#define i_key_str
#define i_val_str
#include <stc/cmap.h>

// Type inference lets us omit an explicit type signature (which
// would be `HashMap<String, String>` in this example).
int main()
{
    cmap_str book_reviews = {0};

    // Review some books.
    cmap_str_emplace(&book_reviews,
        "Adventures of Huckleberry Finn",
        "My favorite book."
    );
    cmap_str_emplace(&book_reviews,
        "Grimms' Fairy Tales",
        "Masterpiece."
    );
    cmap_str_emplace(&book_reviews,
        "Pride and Prejudice",
        "Very enjoyable"
    );
    cmap_str_insert(&book_reviews,
        cstr_lit("The Adventures of Sherlock Holmes"),
        cstr_lit("Eye lyked it alot.")
    );

    // Check for a specific one.
    // When collections store owned values (String), they can still be
    // queried using references (&str).
    if (cmap_str_contains(&book_reviews, "Les Misérables")) {
        printf("We've got %" c_ZI " reviews, but Les Misérables ain't one.",
                    cmap_str_size(&book_reviews));
    }

    // oops, this review has a lot of spelling mistakes, let's delete it.
    cmap_str_erase(&book_reviews, "The Adventures of Sherlock Holmes");

    // Look up the values associated with some keys.
    const char* to_find[] = {"Pride and Prejudice", "Alice's Adventure in Wonderland"};
    c_forrange (i, c_arraylen(to_find)) {
        const cmap_str_value* b = cmap_str_get(&book_reviews, to_find[i]);
        if (b)
            printf("%s: %s\n", cstr_str(&b->first), cstr_str(&b->second));
        else
            printf("%s is unreviewed.\n", to_find[i]);
    }

    // Look up the value for a key (will panic if the key is not found).
    printf("Review for Jane: %s\n", cstr_str(cmap_str_at(&book_reviews, "Pride and Prejudice")));

    // Iterate over everything.
    c_forpair (book, review, cmap_str, book_reviews) {
        printf("%s: \"%s\"\n", cstr_str(_.book), cstr_str(_.review));
    }

    cmap_str_drop(&book_reviews);
}
