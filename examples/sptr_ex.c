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

void Song_del(Song* s) {
    printf("del: %s\n", s->title.str);
    c_del(cstr, &s->artist, &s->title);
}

#define i_val Song
#define i_cmp c_no_compare
#define i_del Song_del
#define i_tag song
#include <stc/csptr.h> // define csptr_song

#define i_val_csptr csptr_song
#define i_tag song
#include <stc/cvec.h>

void example3()
{
    c_auto (cvec_song, v, v2)
    {
        c_apply(cvec_song, push_back, &v, {
            csptr_song_make(Song_from("Bob Dylan", "The Times They Are A Changing")),
            csptr_song_make(Song_from("Aretha Franklin", "Bridge Over Troubled Water")),
            csptr_song_make(Song_from("Thalia", "Entre El Mar y Una Estrella"))
        });

        c_foreach (s, cvec_song, v)
            if (!cstr_equalto(s.ref->get->artist, "Bob Dylan"))
                cvec_song_emplace_back(&v2, *s.ref); // note: calls csptr_song_clone()

        c_apply(cvec_song, push_back, &v2, {
            csptr_song_make(Song_from("Michael Jackson", "Billie Jean")),
            csptr_song_make(Song_from("Rihanna", "Stay")),
        });

        c_foreach (s, cvec_song, v2)
            printf("%s - %s: refs %u\n", s.ref->get->artist.str, s.ref->get->title.str,
                                         *s.ref->use_count);
    }
}

int main()
{
    example3();
}
