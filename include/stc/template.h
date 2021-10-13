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

#define Self c_PASTE(i_prefix, i_tag)
#if defined i_valraw && !(defined i_valto && defined i_valfrom)
  #error if i_valraw or i_valto defined, i_valfrom must be defined
#endif
#if defined i_keyraw && !(defined i_keyto && defined i_keyfrom)
  #error if i_keyraw or i_keyto defined, i_keyfrom a must be defined
#endif

#if defined i_key_str || defined i_val_str
  #include "cstr.h"
#endif

#ifdef i_cnt
  #define i_tag i_cnt
  #undef i_prefix
  #define i_prefix
#endif

#ifdef i_key_csptr
  #define i_key i_key_csptr
  #define i_cmp c_PASTE(i_key_csptr, _compare)
  #define i_keydel c_PASTE(i_key_csptr, _del)
  #define i_keyfrom c_PASTE(i_key_csptr, _clone)
#endif

#ifdef i_key_str
  #define i_key     cstr
  #ifndef i_tag
    #define i_tag   str
  #endif
  #define i_cmp     c_rawstr_compare
  #define i_hash    c_rawstr_hash
  #define i_keydel  cstr_del
  #define i_keyfrom cstr_from
  #define i_keyto   cstr_str
  #define i_keyraw  const char*
#endif

#ifdef i_val_csptr
  #define i_val i_val_csptr
  #ifndef i_key
    #define i_cmp c_PASTE(i_val_csptr, _compare)
  #endif
  #define i_valdel c_PASTE(i_val_csptr, _del)
  #define i_valfrom c_PASTE(i_val_csptr, _clone)
#endif

#ifdef i_val_str
  #define i_val     cstr
  #if !defined i_tag && !defined i_key
    #define i_tag   str
  #endif
  #ifndef i_key
    #define i_cmp   c_rawstr_compare
  #endif
  #define i_valdel  cstr_del
  #define i_valfrom cstr_from
  #define i_valto   cstr_str
  #define i_valraw  const char*
#endif

#if defined i_del && defined i_isset
  #define i_keydel i_del
#elif defined i_del && !defined i_key
  #define i_valdel i_del
#elif defined i_del
  #error i_del not supported for maps, define i_keydel / i_valdel instead.
#endif

#ifdef i_key
  #ifdef i_isset
    #define i_val i_key
  #endif
  #ifndef i_tag
    #define i_tag i_key  
  #endif
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
  #ifndef i_keydel
    #define i_keydel c_default_del
  #endif
#elif defined i_isset || defined i_hash || defined i_equ
  #error i_key define is missing.
#endif

#ifndef i_tag
  #define i_tag i_val
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
#ifndef i_valdel
  #define i_valdel c_default_del
#endif
#ifndef i_cmp
  #define i_cmp c_default_compare
#endif

#else // -------------------------------------------------------

#undef i_prefix
#undef i_tag
#undef i_imp
#undef i_fwd
#undef i_cmp
#undef i_del
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
#undef i_key_csptr
#undef i_val_csptr
#undef i_cnt
#undef Self

#undef i_template
#endif
