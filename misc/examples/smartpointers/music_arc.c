// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
#define i_implement
#include <stc/cstr.h>

typedef struct
{
    cstr artist;
    cstr title;
} Song;

int Song_cmp(const Song* x, const Song* y)
    { return cstr_cmp(&x->title, &y->title); }

Song Song_init(const char* artist, const char* title)
    { return c_LITERAL(Song){cstr_from(artist), cstr_from(title)}; }

void Song_drop(Song* s) {
    printf("drop: %s\n", cstr_str(&s->title));
    c_drop(cstr, &s->artist, &s->title);
}

// Define the shared pointer:
#define i_type SongArc
#define i_keyclass Song
#define i_opt c_use_cmp|c_no_hash
#include <stc/carc.h>

// ... and a vector of them
#define i_type SongVec
#define i_keyboxed SongArc // use i_keyboxed on carc / cbox (instead of i_key)
#include <stc/cvec.h>

void example3(void)
{
    SongVec vec1 = c_init(SongVec, {
        Song_init("Bob Dylan", "The Times They Are A Changing"),
        Song_init("Aretha Franklin", "Bridge Over Troubled Water"),
        Song_init("Thalia", "Entre El Mar y Una Estrella")
    });

    SongVec vec2 = {0};
    // Share all entries in vec with vec2, except Bob Dylan.
    c_foreach (s, SongVec, vec1)
        if (!cstr_equals(&s.ref->get->artist, "Bob Dylan"))
            SongVec_push(&vec2, SongArc_clone(*s.ref));

    // Add a few more to vec2. We can use emplace when creating new entries
    // Emplace calls SongArc_from() on the argument to create the Arc:
    SongVec_emplace(&vec2, Song_init("Michael Jackson", "Billie Jean"));
    SongVec_emplace(&vec2, Song_init("Rihanna", "Stay"));

    // We now have two vectors with some shared, some unique entries.
    c_forlist (i, SongVec, {vec1, vec2}) {
        puts("VEC:");
        c_foreach (s, SongVec, *i.ref)
            printf("  %s - %s, REFS: %ld\n", cstr_str(&s.ref->get->artist),
                                             cstr_str(&s.ref->get->title),
                                             *s.ref->use_count);
    }
    c_drop(SongVec, &vec1, &vec2);
}

int main(void)
{
    example3();
}