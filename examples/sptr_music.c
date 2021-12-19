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
    printf("drop: %s\n", s->title.str);
    c_drop(cstr, &s->artist, &s->title);
}

#define i_val Song
#define i_drop Song_drop
#define i_opt c_no_cmp
#define i_tag song
#include <stc/csptr.h> // define csptr_song

#define i_val_bind csptr_song
#define i_tag song
#include <stc/cvec.h>

void example3()
{
    c_auto (cvec_song, v, v2)
    {
        c_apply(cvec_song, push_back, &v, {
            csptr_song_new(Song_new("Bob Dylan", "The Times They Are A Changing")),
            csptr_song_new(Song_new("Aretha Franklin", "Bridge Over Troubled Water")),
            csptr_song_new(Song_new("Thalia", "Entre El Mar y Una Estrella"))
        });

        c_foreach (s, cvec_song, v)
            if (!cstr_equals(s.ref->get->artist, "Bob Dylan"))
                cvec_song_emplace_back(&v2, *s.ref); // note: calls csptr_song_clone()

        c_apply(cvec_song, push_back, &v2, {
            csptr_song_new(Song_new("Michael Jackson", "Billie Jean")),
            csptr_song_new(Song_new("Rihanna", "Stay")),
        });

        c_foreach (s, cvec_song, v2)
            printf("%s - %s: refs %lu\n", s.ref->get->artist.str, s.ref->get->title.str,
                                         *s.ref->use_count);
    }
}

int main()
{
    example3();
}
