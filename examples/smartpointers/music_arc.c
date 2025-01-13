// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160

// Advanced example.

#include "stc/cstr.h"
#include "stc/algorithm.h"

typedef struct {
    cstr artist;
    cstr title;
} Song;

// Make Song a "class" by defining _clone and _drop "members":
Song Song_init(const char* artist, const char* title)
    { return (Song){cstr_from(artist), cstr_from(title)}; }

Song Song_clone(Song s) {
    s.artist = cstr_clone(s.artist);
    s.title = cstr_clone(s.title);
    return s;
}

void Song_drop(Song* self) {
    printf("drop: %s\n", cstr_str(&self->title));
    c_drop(cstr, &self->artist, &self->title);
}

// Define a keyraw conversion type, SongView:
typedef struct {
    const char* artist;
    const char* title;
} SongView;

inline static bool SongView_cmp(const SongView* xw, const SongView* yw)
    { int c = strcmp(xw->artist, yw->artist); return c ? c : strcmp(xw->title, yw->title); }

#define SongView_eq(x, y) SongView_cmp(x, y)==0

inline static size_t SongView_hash(const SongView* xw)
    { return c_hash_mix(c_hash_str(xw->artist), c_hash_str(xw->title)); }


// Define the shared pointer type SongArc and conversion functions to SongView:
#define i_type SongArc
#define i_keyclass Song      // binds Song_clone(), Song_drop()
#define i_rawclass SongView  // Element view type
#define i_keytoraw(x) ((SongView){.artist=cstr_str(&x->artist), .title=cstr_str(&x->title)})
#define i_keyfrom(sw) ((Song){.artist=cstr_from(sw.artist), .title=cstr_from(sw.title)})
#include "stc/arc.h"

// Create a set of SongArc
#define i_type SongSet
#define i_keypro SongArc // use i_keypro when key is an arc type, instead of i_key
#include "stc/hset.h"

void example3(void)
{
    SongSet set1 = c_make(SongSet, {
        (SongView){"Bob Dylan", "The Times They Are A Changing"},
        (SongView){"Aretha Franklin", "Bridge Over Troubled Water"},
        (SongView){"Thalia", "Entre El Mar y Una Estrella"},
    });

    SongSet set2 = {0};
    // Share all entries in set1 with set2. Copy arc => share.
    c_append(SongSet, &set2, set1);

    // Add a few more SongArcs to set2.
    SongSet_emplace(&set2, (SongView){"Bob Dylan", "The Times They Are A Changing"});
    SongSet_emplace(&set2, (SongView){"Michael Jackson", "Billie Jean"});
    /*
    // The previous line is identical to:
        SongSet_insert(&set2, SongArc_make((Song){cstr_lit("Michael Jackson"), cstr_lit("Billie Jean")}));
    // And similar to:
        Song* tmp = malloc(sizeof *tmp);
        tmp->artist = cstr_lit("Michael Jackson");
        tmp->title = cstr_lit("Billie Jean");
        SongSet_insert(&set2, SongArc_from_ptr(tmp));
    // Note that attemting to insert an existing element, insert will "drop"/free the given inputs.
    */
    // We now have two sets with some shared, some unique entries.
    // Remove "Thalia" from set1. Song is not destroyed, there is still one reference in set2:
    SongSet_erase(&set1, (SongView){"Thalia", "Entre El Mar y Una Estrella"});

    int n = 0;
    for (c_items(i, SongSet, {set1, set2})) {
        printf("SET%d:\n", ++n);
        for (c_each(s, SongSet, *i.ref))
            printf("  %s (%s), REFS: %ld\n", cstr_str(&s.ref->get->title),
                                             cstr_str(&s.ref->get->artist),
                                             *s.ref->use_count);
    }
    const SongArc* found = SongSet_get(&set2, (SongView){"Aretha Franklin", "Bridge Over Troubled Water"});
    if (found) printf("FOUND: %s\n", cstr_str(&found->get->title));

    c_drop(SongSet, &set1, &set2);
}


int main(void)
{
    example3();
}
