// https://shadow.github.io/docs/rust/regex/struct.Regex.html#example-6
#include <stc/cregex.h>
#include <stc/csview.h>
#include <stdio.h>

typedef struct {csview name; int year; } Movie;

static inline bool Movie_eq(const Movie* a, const Movie* b)
    { return csview_eq(&a->name, &b->name) && a->year == b->year; }

#define T Movies, Movie
#define i_eq Movie_eq
#include <stc/stack.h>

int main(void) {
    cregex re = cregex_from("'([^']+)'\\s+\\(([0-9][0-9][0-9][0-9])\\)");
    c_assert(!re.error);
    const char* hay = "'Citizen Kane' (1941), 'The Wizard of Oz' (1939), 'M' (1931).";
    Movies movies = {0};

    for (c_each_match(c, &re, hay)) {
        Movies_push(&movies, (Movie){c.match[1], atoi(c.match[2].buf)});
    }

    Movies answer = c_make(Movies, {
        {c_sv("Citizen Kane"), 1941},
        {c_sv("The Wizard of Oz"), 1939},
        {c_sv("M"), 1931},
    });
    c_assert(Movies_eq(&movies, &answer));

    for (c_each_ref(e, Movies, movies))
        printf(c_svfmt ", %d\n", c_svarg(e->name), e->year);

    c_drop(Movies, &movies, &answer);
}
