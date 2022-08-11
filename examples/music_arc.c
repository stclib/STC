// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
#include <stc/cstr.h>

struct Song
{
    cstr artist;
    cstr title;
} typedef Song;

Song Song_from(const char* artist, const char* title)
    { return (Song){cstr_from(artist), cstr_from(title)}; }

void Song_drop(Song* s) {
    printf("drop: %s\n", cstr_str(&s->title));
    c_drop(cstr, &s->artist, &s->title);
}

#define i_type SongArc
#define i_val Song
#define i_valdrop Song_drop
#define i_opt c_no_cmp
#include <stc/carc.h>

#define i_type SongVec
#define i_val_arcbox SongArc
#include <stc/cvec.h>

void example3()
{
    c_auto (SongVec, vec, vec2)
    {
        c_forarray (SongArc, v, {
            SongArc_make(Song_from("Bob Dylan", "The Times They Are A Changing")),
            SongArc_make(Song_from("Aretha Franklin", "Bridge Over Troubled Water")),
            SongArc_make(Song_from("Thalia", "Entre El Mar y Una Estrella"))
        }) SongVec_push_back(&vec, *v);

        c_foreach (s, SongVec, vec)
            if (!cstr_equals(s.ref->get->artist, "Bob Dylan"))
                SongVec_push_back(&vec2, SongArc_clone(*s.ref));

        SongVec_push_back(&vec2, SongArc_make(Song_from("Michael Jackson", "Billie Jean")));
        SongVec_push_back(&vec2, SongArc_make(Song_from("Rihanna", "Stay")));

        c_forarray (SongVec, v, {vec, vec2}) {
            puts("VEC:");
            c_foreach (s, SongVec, *v)
                printf("  %s - %s, REFS: %lu\n", cstr_str(&s.ref->get->artist),
                                                 cstr_str(&s.ref->get->title),
                                                 *s.ref->use_count);
        }
    }
}

int main()
{
    example3();
}
