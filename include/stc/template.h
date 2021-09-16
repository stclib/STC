/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef i_template
#define i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define cx_memb(name) c_PASTE(Self, name)
  #define Self c_PASTE(i_prefix, i_tag)
  // typedef container types defined in forward.h. VC requires c_EXPAND.
  #define cx_deftypes(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  
  #define cx_value_t cx_memb(_value_t)
  #define cx_key_t cx_memb(_key_t)
  #define cx_mapped_t cx_memb(_mapped_t)
  #define cx_rawvalue_t cx_memb(_rawvalue_t)
  #define cx_rawkey_t cx_memb(_rawkey_t)
  #define cx_rawmapped_t cx_memb(_rawmapped_t)
  #define cx_iter_t cx_memb(_iter_t)
  #define cx_result_t cx_memb(_result_t)
  #define cx_node_t cx_memb(_node_t)
  #define cx_size_t cx_memb(_size_t)
#endif

#ifdef F_tag
  #define i_tag F_tag
  #define i_fwd
#endif

#ifdef i_key_str
  #define i_key     cstr
  #define i_cmp     c_rawstr_compare
  #define i_equ     c_rawstr_equals
  #define i_hash    c_rawstr_hash
  #define i_keydel  cstr_del
  #define i_keyfrom cstr_from
  #define i_keyto   cstr_str
  #define i_keyraw  const char*
#endif
#ifdef i_val_str
  #define i_val     cstr
  #ifndef i_key
  #define i_cmp     c_rawstr_compare
  #endif
  #define i_valdel  cstr_del
  #define i_valfrom cstr_from
  #define i_valto   cstr_str
  #define i_valraw  const char*
#endif
#if defined i_key && !defined i_val
  #define i_val i_key
#endif
#if !defined i_tag && defined i_key_str
  #define i_tag str
#elif !defined i_tag && defined i_key
  #define i_tag i_key
#elif !defined i_tag && defined i_val_str
  #define i_tag str
#elif !defined i_tag && defined i_val
  #define i_tag i_val
#endif

#if (defined i_valto ^ defined i_valraw) || (defined i_valraw && !defined i_valfrom)
  #error if i_valraw defined, both i_valfrom and i_valto must be defined
#endif
#if (defined i_keyto ^ defined i_keyraw) || (defined i_keyraw && !defined i_keyfrom)
  #error if i_keyraw defined, both i_keyfrom and i_keyto must be defined
#endif

#ifdef i_key
  #if !defined i_keyfrom && defined i_keydel
    #define i_keyfrom c_no_clone
  #elif !defined i_keyfrom
    #define i_keyfrom c_default_fromraw
  #endif
  #ifndef i_keyraw
    #define i_keyraw i_key
    #define i_keyto c_default_toraw
  #endif
  #if !defined i_equ && defined i_cmp
    #define i_equ !i_cmp
  #elif !defined i_equ
    #define i_equ c_default_equals
  #endif
  #ifndef i_hash
    #define i_hash c_default_hash
  #endif
#endif

#if !defined i_valfrom && defined i_valdel
  #define i_valfrom c_no_clone
#elif !defined i_valfrom
  #define i_valfrom c_default_fromraw
#endif
#ifndef i_valraw
  #define i_valraw i_val
  #define i_valto c_default_toraw
#endif
#ifndef i_keydel
  #define i_keydel c_default_del
#endif
#ifndef i_valdel
  #define i_valdel c_default_del
#endif
#ifndef i_cmp
  #define i_cmp c_default_compare
#endif

#else // -------------------------------------------------------

#undef i_prefix
#undef i_tag
#undef F_tag
#undef i_imp
#undef i_fwd
#undef i_cmp
#undef i_equ
#undef i_hash
#undef i_val
#undef i_val_str
#undef i_valdel
#undef i_valfrom
#undef i_valto
#undef i_valraw
#undef i_key
#undef i_key_str
#undef i_keydel
#undef i_keyfrom
#undef i_keyto
#undef i_keyraw

#undef i_template
#endif
