/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// This is part of implementing serialization in C.
#ifndef STC_CSERDE_ARCHIVE_H_
#define STC_CSERDE_ARCHIVE_H_

#include <stdio.h>
#include "../types.h"

// used for both ptr->intptr and intptr->ptr
c_declare_hashmap(cserde_table, intptr_t, intptr_t); 

typedef struct cserde_archive {
    FILE* fp;
    cserde_table table; // ptr <=> oid mapping
    bool first;         // at first item in object?
    int16_t level;      // print nesting level
    int next_oid;       // object id
} cserde_archive;

extern void cserde_archive_reset(cserde_archive* ar, FILE* fp);
extern void cserde_archive_drop(cserde_archive* ar);
extern bool cserde_save_pointer(const char* name, const void* field,
                                int flag, cserde_archive* ar);

static inline _Bool _to_bool_from_opaque(const void* ref)
    { return *(const _Bool*)ref; }

#ifndef STC_NL
  #define STC_NL "\n"
  #define STC_TB 2
  #define STC_SP " "
#endif
#define STC_IND(ar) ar->level*(STC_TB), "" // fprintf(fp, "%*s", STC_IND) 

#endif
