#ifndef RUST

#include <stc/cmap.h>
#include <stc/cstr.h>

declare_cmap_str();
declare_cset_str();

int main() 
{ 
    // Lets use an explicit type signature (which would
    // be `cmap<String, String>` in this example).
    cmap_str book_reviews = cmap_ini; 
    cset_str set = cset_ini;
    cset_str_put(&set, "Hello");
    cset_str_put(&set, "You");
    cset_str_put(&set, "Tube");
    c_foreach (i, cset_str, set)
        printf("%s ", i.item->key.str); puts("");

    // Review some books.
   c_push(&book_reviews, cmap_str, c_items(
        {"Adventures of Huckleberry Finn", "My favorite book."},
        {"Grimms' Fairy Tales", "Masterpiece."},
        {"Pride and Prejudice", "Very enjoyable."},
        {"The Adventures of Sherlock Holmes", "Eye lyked it alot."},
    ));        
/*
    cmap_str_insert(&book_reviews,
        "Adventures of Huckleberry Finn",
        "My favorite book."
    );
    cmap_str_insert(&book_reviews,
        "Grimms' Fairy Tales",
        "Masterpiece."
    );
    cmap_str_insert(&book_reviews,
        "Pride and Prejudice",
        "Very enjoyable."
    );
    cmap_str_insert(&book_reviews,
        "The Adventures of Sherlock Holmes",
        "Eye lyked it alot."
    );
*/
    // Check for a specific one.
    // When collections store owned values (String), they can still be
    // queried using references (&str).
    if (! cmap_str_find(&book_reviews, "Les Misérables")) {
        printf("We've got %zu reviews, but Les Misérables ain't one.\n",
               cmap_size(book_reviews));
    }

    // oops, this review has a lot of spelling mistakes, let's delete it.
    cmap_str_erase(&book_reviews, "The Adventures of Sherlock Holmes");

    // Look up the values associated with some keys.
    const char* to_find[] = {"Pride and Prejudice", "Alice's Adventure in Wonderland", NULL};
    for (const char** book = to_find; *book; ++book) {
        cmap_str_entry_t *review = cmap_str_find(&book_reviews, *book);
        if (review) printf("%s: %s\n", *book, review->value.str);
        else        printf("%s is unreviewed.\n", *book);
    }

    // Look up the value for a key (will panic if the key is not found).
    printf("Review for Jane: %s\n", cmap_str_find(&book_reviews, "Pride and Prejudice")->value.str);

    // Iterate over everything.
    c_foreach (i, cmap_str, book_reviews) {
        printf("- %s: \"%s\"\n", i.item->key.str, i.item->value.str);
    }
    cmap_str_destroy(&book_reviews);
}

#else // ======================================================

use std::collections::HashMap;

// Type inference lets us omit an explicit type signature (which
// would be `HashMap<String, String>` in this example).
let mut book_reviews = HashMap::new();

// Review some books.
book_reviews.insert(
    "Adventures of Huckleberry Finn".to_str(),
    "My favorite book.".to_str(),
);
book_reviews.insert(
    "Grimms' Fairy Tales".to_str(),
    "Masterpiece.".to_str(),
);
book_reviews.insert(
    "Pride and Prejudice".to_str(),
    "Very enjoyable.".to_str(),
);
book_reviews.insert(
    "The Adventures of Sherlock Holmes".to_str(),
    "Eye lyked it alot.".to_str(),
);

// Check for a specific one.
// When collections store owned values (String), they can still be
// queried using references (&str).
if !book_reviews.contains_key("Les Misérables") {
    println!("We've got {} reviews, but Les Misérables ain't one.",
             book_reviews.len());
}

// oops, this review has a lot of spelling mistakes, let's delete it.
book_reviews.remove("The Adventures of Sherlock Holmes");

// Look up the values associated with some keys.
let to_find = ["Pride and Prejudice", "Alice's Adventure in Wonderland"];
for &book in &to_find {
    match book_reviews.get(book) {
        Some(review) => println!("{}: {}", book, review),
        None => println!("{} is unreviewed.", book)
    }
}

// Look up the value for a key (will panic if the key is not found).
println!("Review for Jane: {}", book_reviews["Pride and Prejudice"]);

// Iterate over everything.
for (book, review) in &book_reviews {
    println!("{}: \"{}\"", book, review);
}

#endif