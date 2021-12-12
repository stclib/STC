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
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define _cx_self c_PASTE(_i_prefix, i_tag)
  #define _cx_memb(name) c_PASTE(_cx_self, name)
  #define _cx_deftypes(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  #define _cx_value _cx_memb(_value)
  #define _cx_key _cx_memb(_key)
  #define _cx_mapped _cx_memb(_mapped)
  #define _cx_rawvalue _cx_memb(_rawvalue)
  #define _cx_rawkey _cx_memb(_rawkey)
  #define _cx_rawmapped _cx_memb(_rawmapped)
  #define _cx_iter _cx_memb(_iter)
  #define _cx_result _cx_memb(_result)
  #define _cx_node _cx_memb(_node)
  #define _cx_size _cx_memb(_size_t)
#endif

#if defined i_valraw && !(defined i_valto && defined i_valfrom)
  #error "if i_valraw or i_valto defined, i_valfrom must be defined"
#endif
#if defined i_keyraw && !(defined i_keyto && defined i_keyfrom)
  #error "if i_keyraw or i_keyto defined, i_keyfrom a must be defined"
#endif
#ifdef i_key_csptr // [deprecated]
  #define i_key_ref i_key_csptr
  #error "i_key_csptr no longer supported: use new name i_key_ref"
#endif
#ifdef i_val_csptr // [deprecated]
  #define i_val_ref i_val_csptr
  #error "i_val_csptr no longer supported: use new name i_val_ref"
#endif
#ifdef i_cnt // [deprecated]
  #define i_type i_cnt
  #error "i_cnt no longer supported: use new name i_type"
#endif

#ifdef i_type
  #define i_tag i_type
  #undef _i_prefix
  #define _i_prefix
#endif

#if defined i_key_str || defined i_val_str
  #include "cstr.h"
#endif

#if defined i_key_ref
  #define i_key i_key_ref
  #define i_keyfrom c_PASTE(i_key, _clone)
  #ifndef i_tag
    #define i_tag ref
  #endif
  #ifndef i_cmp
    #define i_cmp c_PASTE(i_key, _compare)
  #endif
  #ifndef i_hash
    #define i_hash c_PASTE(i_key, _hash)
  #endif
  #ifndef i_keydel
    #define i_keydel c_PASTE(i_key, _del)
  #endif

#elif defined i_key_str

  #define i_key cstr
  #define i_keyfrom cstr_from
  #define i_keyto cstr_str
  #define i_keyraw const char*
  #ifndef i_tag
    #define i_tag str
  #endif
  #ifndef i_cmp  
    #define i_cmp c_rawstr_compare
  #endif
  #ifndef i_hash
    #define i_hash c_rawstr_hash
  #endif
  #if !defined i_keydel
    #define i_keydel cstr_del
  #endif
#endif

#if defined i_val_ref
  #define i_val i_val_ref
  #define i_valfrom c_PASTE(i_val, _clone)
  #if !defined i_tag && !defined i_key
    #define i_tag ref
  #endif
  #if !defined i_cmp && !defined i_key
    #define i_cmp c_PASTE(i_val, _compare)
  #endif
  #if !defined i_valdel && !defined i_del
    #define i_valdel c_PASTE(i_val, _del)
  #endif

#elif defined i_val_str

  #define i_val     cstr
  #define i_valfrom cstr_from
  #define i_valto   cstr_str
  #define i_valraw  const char*
  #if !defined i_tag && !defined i_key
    #define i_tag   str
  #endif
  #if !defined i_cmp && !defined i_key
    #define i_cmp   c_rawstr_compare
  #endif
  #if !defined i_valdel && !defined i_del
    #define i_valdel cstr_del
  #endif
#endif

#if defined i_del && defined i_isset
  #define i_keydel i_del
#elif defined i_del && !defined i_key
  #define i_valdel i_del
#elif defined i_del
  #error "i_del not supported for maps, define i_keydel / i_valdel instead."
#endif
#if defined i_from && defined i_isset
  #define i_keyfrom i_from
#elif defined i_from && !defined i_key
  #define i_valfrom i_from
#elif defined i_from
  #error "i_from not supported for maps, define i_keyfrom / i_valfrom instead."
#endif

#ifdef i_key
  #ifdef _i_isset
    #define i_val i_key
  #endif
  #ifndef i_tag
    #define i_tag i_key  
  #endif
  #if !defined _i_has_internal_clone && defined i_keydel && !defined i_keyfrom && !c_option(c_no_clone)
    #error "i_keydel defined but not i_keyfrom (e.g. as c_default_clone), or no 'i_opt c_no_clone'"
  #endif
  #if !defined i_keyfrom
    #define i_keyfrom c_default_clone
  #endif
  #ifndef i_keyraw
    #define i_keyraw i_key
    #define i_keyto c_default_toraw
  #endif
  #if !defined i_equ && defined i_cmp
    #define i_equ !i_cmp
  #elif !defined i_equ
    #define i_equ c_default_equalto
  #endif
  #ifndef i_keydel
    #define i_keydel c_default_del
  #endif
#elif defined _i_isset
  #error "i_key define is missing."
#endif

#ifndef i_tag
  #define i_tag i_val
#endif
#if !defined _i_has_internal_clone && defined i_valdel && !defined i_valfrom && !c_option(c_no_clone)
  #error "i_valdel/i_del defined but not i_valfrom (e.g. as c_default_clone), or no 'i_opt c_no_clone'"
#endif
#if !defined i_valfrom
  #define i_valfrom c_default_clone
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
#ifndef i_hash
  #define i_hash c_default_hash
#endif

#else // -------------------------------------------------------

#undef i_type
#undef i_tag
#undef i_imp
#undef i_opt
#undef i_cmp
#undef i_del
#undef i_equ
#undef i_hash
#undef i_from

#undef i_val
#undef i_val_str
#undef i_val_ref
#undef i_valdel
#undef i_valraw
#undef i_valfrom
#undef i_valto

#undef i_key
#undef i_key_str
#undef i_key_ref
#undef i_keydel
#undef i_keyraw
#undef i_keyfrom
#undef i_keyto

#undef _i_prefix
#undef _i_has_internal_clone
#undef _i_template
#endif
