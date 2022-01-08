#ifndef STC_STRINGS_INCLUDED
#define STC_STRINGS_INCLUDED

#include "cstr.h"
#include "csview.h"
#include <ctype.h>

/* cstr interaction with csview: */

STC_INLINE csview       csview_from_s(cstr s)
                            { return c_make(csview){s.str, _cstr_rep(&s)->size}; }

STC_INLINE cstr         cstr_from_v(csview sv)
                            { return cstr_from_n(sv.str, sv.size); }
STC_INLINE cstr         cstr_from_replace_all_v(csview sv, csview find, csview repl)
                            { return cstr_from_replace_all(sv.str, sv.size, find.str, find.size,
                                                           repl.str, repl.size); }
STC_INLINE csview       cstr_to_v(const cstr* self)
                            { return c_make(csview){self->str, _cstr_rep(self)->size}; }
STC_INLINE csview       cstr_substr(cstr s, intptr_t pos, size_t n)
                            { return csview_substr(csview_from_s(s), pos, n); }
STC_INLINE csview       cstr_slice(cstr s, intptr_t p1, intptr_t p2)
                            { return csview_slice(csview_from_s(s), p1, p2); }
STC_INLINE cstr*        cstr_assign_v(cstr* self, csview sv)
                            { return cstr_assign_n(self, sv.str, sv.size); }
STC_INLINE cstr*        cstr_append_v(cstr* self, csview sv)
                            { return cstr_append_n(self, sv.str, sv.size); }
STC_INLINE void         cstr_insert_v(cstr* self, size_t pos, csview sv)
                            { cstr_replace_n(self, pos, 0, sv.str, sv.size); }
STC_INLINE void         cstr_replace_v(cstr* self, size_t pos, size_t len, csview sv)
                            { cstr_replace_n(self, pos, len, sv.str, sv.size); }
STC_INLINE bool         cstr_equals_v(cstr s, csview sv)
                            { return sv.size == cstr_size(s) && !memcmp(s.str, sv.str, sv.size); }
STC_INLINE size_t       cstr_find_v(cstr s, csview needle)
                            { char* res = c_strnstrn(s.str, needle.str, cstr_size(s), needle.size);
                              return res ? res - s.str : cstr_npos; }
STC_INLINE bool         cstr_contains_v(cstr s, csview needle)
                            { return c_strnstrn(s.str, needle.str, cstr_size(s), needle.size) != NULL; }
STC_INLINE bool         cstr_starts_with_v(cstr s, csview sub)
                            { if (sub.size > cstr_size(s)) return false;
                              return !memcmp(s.str, sub.str, sub.size); }
STC_INLINE bool         cstr_ends_with_v(cstr s, csview sub)
                            { if (sub.size > cstr_size(s)) return false;
                              return !memcmp(s.str + cstr_size(s) - sub.size, sub.str, sub.size); }

#endif
#undef i_opt
