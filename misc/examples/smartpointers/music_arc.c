// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160

// Highly advanced example.

#define i_implement
#include "stc/cstr.h"

struct {
    cstr artist;
    cstr title;
} typedef Song;

// Song "class" members (only _drop is actually used in this example):
Song Song_init(const char* artist, const char* title)
    { return (Song){cstr_from(artist), cstr_from(title)}; }

bool Song_eq(const Song* x, const Song* y)
    { return cstr_eq(&x->artist, &y->artist) && cstr_eq(&x->title, &y->title); }

Song Song_clone(Song s) {
    s.artist = cstr_clone(s.artist);
    s.title = cstr_clone(s.title);
    return s;
}

void Song_drop(Song* self) {
    printf("drop: %s\n", cstr_str(&self->title));
    c_drop(cstr, &self->artist, &self->title);
}

// Define the shared pointer with a keyraw conversion type, SongView:
struct {
    const char* artist;
    const char* title;
} typedef SongView;

#define i_type SongArc
#define i_key_class Song
#define i_keyraw SongView
#define i_keyto(x) ((SongView){.artist=cstr_str(&x->artist), .title=cstr_str(&x->title)})
#define i_keyfrom(sw) ((Song){.artist=cstr_from(sw.artist), .title=cstr_from(sw.title)})
// When commenting the next two lines (or either), containers will use object address comparison and hashing
// provided by arc instead of object content comparison (i.e. "Billie Jean" will appear more than once):
#define i_eq(xw, yw) (strcmp(xw->artist, yw->artist)==0 && strcmp(xw->title, yw->title)==0)
#define i_hash(xw) c_hash_mix(c_hash_str(xw->artist), c_hash_str(xw->title))
#include "stc/arc.h"

// Create a set of SongArc
#define i_type SongSet
#define i_key_arcbox SongArc // use i_key_arcbox on arc / box (not i_key)
#include "stc/hset.h"
#include "stc/algorithm.h"


void example3(void)
{
    SongSet set1 = c_init(SongSet, {
        (SongView){"Bob Dylan", "The Times They Are A Changing"},
        (SongView){"Aretha Franklin", "Bridge Over Troubled Water"},
        (SongView){"Thalia", "Entre El Mar y Una Estrella"},
    });

    SongSet set2 = {0};
    // Share all entries in set1 with set2. Copy arc => share.
    c_copy(SongSet, set1, &set2);

    // Add a few more SongArcs to set2.
    SongSet_emplace(&set2, (SongView){"Bob Dylan", "The Times They Are A Changing"});
    SongSet_emplace(&set2, (SongView){"Michael Jackson", "Billie Jean"});
    /*
    // The previous line is identical to:
        SongSet_insert(&set2, SongArc_make((Song){cstr_from("Michael Jackson"), cstr_from("Billie Jean")}));
    // And similar, but not identical to (i.e. SongArc_make() combines malloc of Song and reference counter):
        Song* tmp = malloc(sizeof *tmp);
        tmp->artist = cstr_from("Michael Jackson");
        tmp->title = cstr_from("Billie Jean");
        SongSet_insert(&set2, SongArc_from_ptr(tmp));
    // NB: In the last two failed attempt to insert an "existing" element, SongSet will "drop" the given inputs.
    */
    // We now have two sets with some shared, some unique entries.
    // Remove "Thalia" from set1. Song is not destroyed, there is still one reference in set2:
    SongSet_erase(&set1, (SongView){"Thalia", "Entre El Mar y Una Estrella"});

    int n = 0;
    c_forlist (i, SongSet, {set1, set2}) {
        printf("SET%d:\n", ++n);
        c_foreach (s, SongSet, *i.ref)
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