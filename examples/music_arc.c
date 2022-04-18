// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160

#include <stc/cstr.h>

struct Song
{
    cstr artist;
    cstr title;
} typedef Song;

Song Song_new(const char* artist, const char* title)
    { return (Song){cstr_from(artist), cstr_from(title)}; }

void Song_drop(Song* s) {
    printf("drop: %s\n", cstr_str(&s->title));
    c_drop(cstr, &s->artist, &s->title);
}

#define i_type SongPtr
#define i_val Song
#define i_valdrop Song_drop
#define i_opt c_no_cmp
#include <stc/carc.h>

#define i_type SongVec
#define i_val_arcbox SongPtr
#include <stc/cvec.h>

void example3()
{
    c_auto (SongVec, vec, vec2)
    {
        c_apply(v, SongVec_push_back(&vec, v), SongPtr, {
            SongPtr_from(Song_new("Bob Dylan", "The Times They Are A Changing")),
            SongPtr_from(Song_new("Aretha Franklin", "Bridge Over Troubled Water")),
            SongPtr_from(Song_new("Thalia", "Entre El Mar y Una Estrella"))
        });

        c_foreach (s, SongVec, vec)
            if (!cstr_equals(s.ref->get->artist, "Bob Dylan"))
                SongVec_push_back(&vec2, SongPtr_clone(*s.ref));

        c_apply(v, SongVec_push_back(&vec2, v), SongPtr, {
            SongPtr_from(Song_new("Michael Jackson", "Billie Jean")),
            SongPtr_from(Song_new("Rihanna", "Stay")),
        });

        c_foreach (s, SongVec, vec2)
            printf("%s - %s: refs %lu\n", cstr_str(&s.ref->get->artist),
                                          cstr_str(&s.ref->get->title),
                                          *s.ref->use_count);
    }
}

int main()
{
    example3();
}
