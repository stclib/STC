// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160

// Advanced example.

#define i_implement
#include "stc/cstr.h"

typedef struct
{
    cstr artist;
    cstr title;
} Song;

Song Song_init(const char* artist, const char* title)
    { return c_LITERAL(Song){cstr_from(artist), cstr_from(title)}; }

bool Song_eq(const Song* x, const Song* y)
    { printf("eq\n"); return cstr_eq(&x->artist, &y->artist) && cstr_eq(&x->title, &y->title); }

uint64_t Song_hash(const Song* s)
    { printf("hash\n"); return c_hash_mix(cstr_hash(&s->artist), cstr_hash(&s->title)); }

void Song_drop(Song* s) {
    printf("drop: %s\n", cstr_str(&s->title));
    c_drop(cstr, &s->artist, &s->title);
}

// Define the shared pointer:
#define i_type SongArc
#define i_key_class Song
#define i_use_eq // Use Song_eq(): enable containers with SongArc elements to compare Song - Song,
                 // otherwise it compares pointers Song* - Song*. (determined at compile-time).
#include "stc/arc.h"

// Create a set of SongArc
#define i_type SongSet
#define i_key_arcbox SongArc // use i_key_arcbox on arc / box (not i_key)
#include "stc/hset.h"
#include "stc/algorithm.h"


void example3(void)
{
    SongSet set1 = c_init(SongSet, {
        Song_init("Bob Dylan", "The Times They Are A Changing"),
        Song_init("Aretha Franklin", "Bridge Over Troubled Water"),
        Song_init("Thalia", "Entre El Mar y Una Estrella")
    });

    SongSet set2 = {0};
    // Share all entries in set1 with set2. Copy arc => share.
    c_copy(SongSet, set1, &set2);

    // Add a few more SongArcs to set2.

    c_forlist (i, Song, {
        Song_init("Michael Jackson", "Billie Jean"),
        Song_init("Bob Dylan", "The Times They Are A Changing"),
    }){
        // Safe: If not inserted, it calls SongArc_drop(i.ref),
        // which also calls Song_drop(i.ref->get):
        SongSet_insert(&set2, SongArc_make(*i.ref));

        // The following may be more efficient, but it requires manually
        // dropping the input "rawtype" Song if it is not inserted. This is because
        // Song is a non-trivial type itself:
        //if (!SongSet_emplace(&set2, *i.ref).inserted) Song_drop(i.ref);
    }

    // We now have two sets with some shared, some unique entries.
    c_forlist (i, SongSet, {set1, set2}) {
        puts("SET:");
        c_foreach (s, SongSet, *i.ref)
            printf("  %s (%s), REFS: %ld\n", cstr_str(&s.ref->get->title),
                                             cstr_str(&s.ref->get->artist),
                                             *s.ref->use_count);
    }
    const SongArc* found = NULL;
    c_scoped (Song s = Song_init("Michael Jackson", "Billie Jean"), Song_drop(&s)) {
        found = SongSet_get(&set2, s);
    }
    if (found) printf("found %s\n", cstr_str(&found->get->title));

    printf("DONE\n");
    c_drop(SongSet, &set1, &set2);
}


int main(void)
{
    example3();
}