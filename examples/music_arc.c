// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
#include <stc/cstr.h>

struct Song
{
    cstr artist;
    cstr title;
} typedef Song;

int Song_cmp(const Song* x, const Song* y)
    { return cstr_cmp(&x->title, &y->title); }

Song Song_from(const char* artist, const char* title)
    { return (Song){cstr_from(artist), cstr_from(title)}; }

void Song_drop(Song* s) {
    printf("drop: %s\n", cstr_str(&s->title));
    c_drop(cstr, &s->artist, &s->title);
}

// Define the reference counted type
#define i_type SongArc
#define i_val Song
#define i_valdrop Song_drop
#define i_cmp Song_cmp
#include <stc/carc.h>

// ... and a vector of it
#define i_type SongVec
#define i_val_arcbox SongArc
#include <stc/cstack.h>

void example3()
{
    c_auto (SongVec, vec1, vec2)
    {
        c_forarray (Song, v, {
            Song_from("Bob Dylan", "The Times They Are A Changing"),
            Song_from("Aretha Franklin", "Bridge Over Troubled Water"),
            Song_from("Thalia", "Entre El Mar y Una Estrella")
        }) SongVec_emplace(&vec1, *v);

        // Share all entries in vec with vec2, except Bob Dylan.
        c_foreach (s, SongVec, vec1)
            if (!cstr_equals(&s.ref->get->artist, "Bob Dylan"))
                SongVec_push(&vec2, SongArc_clone(*s.ref));

        // Add a few more to vec2. We can use emplace when creating new entries
        SongVec_emplace(&vec2, Song_from("Michael Jackson", "Billie Jean"));
        SongVec_emplace(&vec2, Song_from("Rihanna", "Stay"));
        // If we use push, we would need to construct the Arc explicitly (as in c++, make_shared):
        // SongVec_push(&vec2, SongArc_from(Song_from("Rihanna", "Stay")));

        // We now have two vectors with some shared, some unique entries.
        c_forarray (SongVec, v, {vec1, vec2}) {
            puts("VEC:");
            c_foreach (s, SongVec, *v)
                printf("  %s - %s, REFS: %ld\n", cstr_str(&s.ref->get->artist),
                                                 cstr_str(&s.ref->get->title),
                                                 *s.ref->use_count);
        }
    } // because the shared elem. are ref. counted, they are only dropped once here.
}

int main()
{
    example3();
}
