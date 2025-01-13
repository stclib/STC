# STC [UTF8](../include/stc/utf8.h): string functionality
![String](pics/string.jpg)

This contains low-level utf8 functions which are used by [cstr](cstr_api.md) (string), [csview](csview_api.md) (string-view), and [zsview](zsview_api.md) (zero-terminated string-view). A utf8 codepoint is often
referred to as a ***rune***, and can be 1 to 4 bytes long.

## Header file

This header file is rarely needed alone. It is included by all the string/view types mentioned above.

```c++
#include "stc/utf8.h"
```
## Methods
```c++
isize           utf8_count(const char *s);                      // number of utf8 runes in s
isize           utf8_count_n(const char *s, isize nbytes);      // number of utf8 runes within n bytes
isize           utf8_to_index(const char* s, isize u8pos);      // from utf8 pos to byte index
int             utf8_chr_size(const char* s);                   // utf8 character size: 1-4
const char*     utf8_at(const char *s, isize u8pos);            // return the char* at u8pos
csview          utf8_subview(const char* s, isize u8pos, isize u8len); // return a csview as the span

bool            utf8_valid(const char* s);
bool            utf8_valid_n(const char* s, isize nbytes);
uint32_t        utf8_decode(utf8_decode_t *d, uint8_t byte);    // decode next byte to utf8, returns state.
int             utf8_encode(char *out, uint32_t codepoint);     // encode unicode cp to out. returns nbytes.
uint32_t        utf8_peek(const char* s);                       // codepoint value at character pos s
uint32_t        utf8_peek_at(const char* s, isize u8offset);    // codepoint at utf8 offset (may be negative)
int             utf8_icmp(const char* s1, const char* s2);      // case-insensitive char* comparison
int             utf8_icompare(csview s1, csview s2);            // case-insensitive csview comparison

uint32_t        utf8_casefold(uint32_t c);                      // fold to a non-unique lowercase char.
uint32_t        utf8_tolower(uint32_t c);
uint32_t        utf8_toupper(uint32_t c);

bool            utf8_isalpha(uint32_t c);
bool            utf8_isalnum(uint32_t c);
bool            utf8_isword(uint32_t c);
bool            utf8_iscased(uint32_t c);
bool            utf8_isblank(uint32_t c);
bool            utf8_isspace(uint32_t c);
bool            utf8_isupper(uint32_t c);
bool            utf8_islower(uint32_t c);
bool            utf8_isgroup(int group, uint32_t c);
```

## Example: UTF8 iteration and case conversion
```c++
#include "stc/cstr.h"
#include "stc/zsview.h"

int main(void)
{
    zsview say = c_zv("Liberté, égalité, fraternité.");
    printf("%s\n", zs.str);

    for (c_each(i, zsview, say))
        printf(c_svfmt " ", c_svarg(i.chr));
    puts("");

    cstr upper = cstr_toupper_sv(zsview_sv(say));
    printf("%s\n", cstr_str(&upper));
    cstr_drop(&upper);
}
```
Output:
```
Liberté, égalité, fraternité.
L i b e r t é ,   é g a l i t é ,   f r a t e r n i t é .
LIBERTÉ, ÉGALITÉ, FRATERNITÉ.
```
