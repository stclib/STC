/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
#include "stc/common.h"
#include "stc/types.h"

#ifndef i_type
  #error "You must define i_type for the extend functionality"
  #define i_type PLEASE_DEFINE_i_type
#endif

#if defined i_key_cstr
  #define _i_key cstr
#elif defined i_keyclass
  #define _i_key i_keyclass
#elif defined i_key_arcbox
  #define _i_key i_key_arcbox
#elif defined i_key
  #define _i_key i_key
#endif

#ifdef i_val_cstr
  #define _i_val cstr
#elif defined i_valclass
  #define _i_val i_valclass
#elif defined i_val_arcbox
  #define _i_val i_val_arcbox
#elif defined i_val
  #define _i_val i_val
#endif

#ifdef i_base // [deprecated]
  #define i_container i_base
#endif

#if defined _i_key && defined _i_val
  c_JOIN(forward_, i_container)(i_type, _i_key, _i_val);
#elif defined _i_key
  c_JOIN(forward_, i_container)(i_type, _i_key);
#else
  c_JOIN(forward_, i_container)(i_type, _i_val);
#endif

typedef struct c_JOIN(i_type, _ext) {
    i_type get[1]; // must be first!
    i_extend
} c_JOIN(i_type, _ext);

#define c_extend() ((c_JOIN(i_type, _ext)*) self)
// c_extend() is accessible from the following customization macros:
//   i_malloc, i_calloc, i_realloc, i_free: all container types
//   i_cmp: smap and sset types
//   i_hash, i_eq: hmap and hset types
//   i_less: pque types

// flag that container type was forward declared, and don't undef the template parameters:
#define i_opt c_is_forward | c_more
#define _i_inc <stc/i_container.h>
#include _i_inc

// Add a clone function for the extended container struct.
// Assumes that the extended member(s) are POD/trivial type(s).
#ifndef i_no_clone
STC_INLINE c_JOIN(i_type, _ext) _c_MEMB(_ext_clone)(c_JOIN(i_type, _ext) cx) {
    *cx.get = _c_MEMB(_clone)(*cx.get);
    return cx;
}
#endif

#include "priv/template2.h" // undef the template parameters
#undef _i_inc
#undef _i_key
#undef _i_val
#undef i_container
#undef i_base   // [deprecated]
#undef i_extend
#undef c_extend // make it unavailable when irrelevant
