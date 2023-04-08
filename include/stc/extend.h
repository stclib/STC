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
#include <stc/ccommon.h>
#include <stc/forward.h>

#ifdef i_key_str
  #define _i_key cstr
#elif defined i_keyclass
  #define _i_key i_keyclass
#elif defined i_keyboxed
  #define _i_key i_keyboxed
#elif defined i_key
  #define _i_key i_key
#endif

#ifdef i_val_str
  #define _i_val cstr
#elif defined i_valclass
  #define _i_val i_valclass
#elif defined i_valboxed
  #define _i_val i_valboxed
#elif defined i_val
  #define _i_val i_val
#endif

#ifdef _i_key
  c_PASTE(forward_, i_con)(i_type, _i_key, _i_val);
#else
  c_PASTE(forward_, i_con)(i_type, _i_val);
#endif

typedef struct {
    i_extend
    i_type get;
} c_PASTE(i_type, _ext);

#define c_getcon(cptr) c_container_of(cptr, _cx_memb(_ext), get)

#define i_is_forward
#define _i_inc <stc/i_con.h>
#include _i_inc
#undef _i_inc
#undef _i_key
#undef _i_val
#undef i_con
#undef i_extend
