/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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

#ifdef i_key_str
  #define _i_key cstr
#elif defined i_key_class
  #define _i_key i_key_class
#elif defined i_key_arcbox
  #define _i_key i_key_arcbox
#elif defined i_key
  #define _i_key i_key
#endif

#ifdef i_val_str
  #define _i_val cstr
#elif defined i_val_class
  #define _i_val i_val_class
#elif defined i_val_arcbox
  #define _i_val i_val_arcbox
#elif defined i_val
  #define _i_val i_val
#endif

#if defined _i_key && defined _i_val
  c_JOIN(forward_, i_base)(i_type, _i_key, _i_val);
#elif defined _i_key
  c_JOIN(forward_, i_base)(i_type, _i_key);
#else
  c_JOIN(forward_, i_base)(i_type, _i_val);
#endif

typedef struct {
    i_extend
    i_type get;
} c_JOIN(i_type, _ext);

#define c_extend() c_container_of(self, _c_MEMB(_ext), get)
// Note: i_less: c_extend() accessible for cpque types
//       i_cmp: c_extend() accessible for csmap and csset types
//       i_hash/i_eq: c_extend() accessible for cmap and cset types

#define i_is_forward
#define _i_inc <stc/i_base.h>
#include _i_inc
#undef _i_inc
#undef _i_key
#undef _i_val
#undef i_base
#undef i_extend
