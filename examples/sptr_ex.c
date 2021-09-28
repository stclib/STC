// shared_ptr-examples.cpp
// based on https://docs.microsoft.com/en-us/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160

#include <stdio.h>
#include <stc/cstr.h>
#include <stc/forward.h>

forward_csptr(song, struct Song);
struct Test {
    csptr_song song1;
    csptr_song song2;
};

// ...

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
#define F_tag song
#define i_cmp c_no_compare
#define i_del Song_del
#include <stc/csptr.h>

#define i_val_csptr song
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
                cvec_song_emplace_back(&v2, *s.ref); // calls csptr_song_clone()

        cvec_song_push_back(&v2, csptr_song_make(Song_from("Pink Floyd", "Dogs")));

        c_foreach (s, cvec_song, v2)
            printf("%s - %s: refs %u\n", s.ref->get->artist.str, s.ref->get->title.str,
                                         *s.ref->use_count);
    }
}

int main()
{
    example3();
}