// https://doc.rust-lang.org/std/collections/struct.HashMap.html
#include <stc/cstr.h>
#define T Reviews, cstr, cstr, (c_pro_key | c_pro_val)
#include <stc/hashmap.h>

// Type inference lets us omit an explicit type signature (which
// would be `HashMap<String, String>` in this example).
int main(void)
{
    Reviews book_reviews = {0};

    // Review some books.
    Reviews_emplace(&book_reviews,
        "Adventures of Huckleberry Finn",
        "My favorite book."
    );
    Reviews_emplace(&book_reviews,
        "Grimms' Fairy Tales",
        "Masterpiece."
    );
    Reviews_emplace(&book_reviews,
        "Pride and Prejudice",
        "Very enjoyable"
    );
    Reviews_insert(&book_reviews,
        cstr_lit("The Adventures of Sherlock Holmes"),
        cstr_lit("Eye lyked it alot.")
    );

    // Check for a specific one.
    // When collections store owned values (String), they can still be
    // queried using references (&str).
    if (Reviews_contains(&book_reviews, "Les Misérables")) {
        printf("We've got %d reviews, but Les Misérables ain't one.",
                    (int)Reviews_size(&book_reviews));
    }

    // oops, this review has a lot of spelling mistakes, let's delete it.
    Reviews_erase(&book_reviews, "The Adventures of Sherlock Holmes");

    // Look up the values associated with some keys.
    const char* to_find[] = {"Pride and Prejudice", "Alice's Adventure in Wonderland"};
    for (c_range(i, c_countof(to_find))) {
        const Reviews_value* b = Reviews_get(&book_reviews, to_find[i]);
        if (b)
            printf("%s: %s\n", cstr_str(&b->first), cstr_str(&b->second));
        else
            printf("%s is unreviewed.\n", to_find[i]);
    }

    // Look up the value for a key (will panic if the key is not found).
    printf("Review for Jane: %s\n", cstr_str(Reviews_at(&book_reviews, "Pride and Prejudice")));

    // Iterate over everything.
    for (c_each_kv(book, review, Reviews, book_reviews)) {
        printf("%s: \"%s\"\n", cstr_str(book), cstr_str(review));
    }

    Reviews_drop(&book_reviews);
}
