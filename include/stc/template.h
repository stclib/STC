/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
  #define _cx_raw _cx_memb(_raw)
  #define _cx_rawkey _cx_memb(_rawkey)
  #define _cx_rawmapped _cx_memb(_rawmapped)
  #define _cx_iter _cx_memb(_iter)
  #define _cx_result _cx_memb(_result)
  #define _cx_node _cx_memb(_node)
  #define _cx_size _cx_memb(_size_t)
#endif

#if defined i_cnt || defined i_equ // [deprecated]
  #define i_type i_cnt
  #error "i_cnt and i_equ no longer supported: use new name i_type / i_eq"
#endif

#ifdef i_type
  #define i_tag i_type
  #undef _i_prefix
  #define _i_prefix
#endif

#if defined i_key_str || defined i_val_str
  #include "cstr.h"
#endif

#ifdef i_key_str
  #define i_key_bind cstr
  #define i_keyraw crawstr
  #ifndef i_tag
    #define i_tag str
  #endif
#elif defined i_key_sptr
  #define i_key_bind i_key_sptr
  #define i_keyraw c_PASTE(i_key_sptr, _value)
#endif

#ifdef i_key_bind
  #define i_key i_key_bind
  #ifndef i_keyraw
    #ifndef i_keyfrom
      #define i_keyfrom c_PASTE(i_key, _clone)
    #endif
  #else
    #ifndef i_keyfrom
      #define i_keyfrom c_PASTE(i_key, _from)
    #endif
    #ifndef i_keyto
      #define i_keyto c_PASTE(i_key, _toraw)
    #endif
  #endif
  #ifndef i_cmp
    #define i_cmp c_PASTE(i_keyraw, _cmp)
  #endif
  #ifndef i_eq
    #define i_eq c_PASTE(i_keyraw, _eq)
  #endif
  #ifndef i_hash
    #define i_hash c_PASTE(i_keyraw, _hash)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_PASTE(i_key, _drop)
  #endif
#endif

#if defined i_keyraw && !(defined i_keyto && defined i_keyfrom)
  #error "if i_keyraw defined, i_keyfrom and i_keyto must be defined"
#endif

/* Resolve i_drop and i_from here */
#if defined i_drop && defined i_isset
  #define i_keydrop i_drop
#elif defined i_drop && !defined i_key
  #define i_valdrop i_drop
#elif defined i_drop
  #error "i_drop not supported for maps, define i_keydrop / i_valdrop instead."
#endif
#if defined i_from && defined i_isset
  #define i_keyfrom i_from
#elif defined i_from && !defined i_key
  #define i_valfrom i_from
#elif defined i_from
  #error "i_from not supported for maps, define i_keyfrom / i_valfrom instead."
#endif

#ifdef i_val_str
  #define i_val_bind cstr
  #define i_valraw crawstr
  #if !defined i_tag && !defined i_key
    #define i_tag str
  #endif
#elif defined i_val_sptr
  #define i_val_bind i_val_sptr
  #define i_valraw c_PASTE(i_val_sptr, _value)
#endif

#ifdef i_val_bind
  #define i_val i_val_bind
  #ifndef i_valraw
    #ifndef i_valfrom
      #define i_valfrom c_PASTE(i_val, _clone)
    #endif
  #else
    #ifndef i_valfrom
      #define i_valfrom c_PASTE(i_val, _from)
    #endif
    #ifndef i_valto
      #define i_valto c_PASTE(i_val, _toraw)
    #endif
  #endif
  #if !defined i_cmp && !defined i_key
    #define i_cmp c_PASTE(i_valraw, _cmp)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_PASTE(i_val, _drop)
  #endif
#endif

#if defined i_valraw && !(defined i_valto && defined i_valfrom)
  #error "if i_valraw defined, i_valfrom and i_valto must be defined"
#endif

#ifdef i_key
  #ifdef _i_isset
    #define i_val i_key
  #endif
  #ifndef i_tag
    #define i_tag i_key  
  #endif
  #if !defined _i_has_internal_clone && defined i_keydrop && !defined i_keyfrom && !c_option(c_no_clone)
    #error "i_keydrop defined but not i_keyfrom (e.g. as c_default_from), or no 'i_opt c_no_clone'"
  #endif
  #if !defined i_keyfrom
    #define i_keyfrom c_default_from
  #endif
  #ifndef i_keyraw
    #define _i_no_keyraw
    #define i_keyraw i_key
    #define i_keyto c_default_toraw
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_default_drop
  #endif
#elif defined _i_isset
  #error "i_key define is missing."
#endif

#ifndef i_tag
  #define i_tag i_val
#endif
#if !defined _i_has_internal_clone && defined i_valdrop && !defined i_valfrom && !c_option(c_no_clone)
  #error "i_valdrop/i_drop defined but not i_valfrom (e.g. as c_default_from), or no 'i_opt c_no_clone'"
#endif
#if !defined i_valfrom
  #define i_valfrom c_default_from
#endif
#ifndef i_valraw
  #if !defined i_key || defined _i_no_keyraw
    #define _i_no_raw
  #endif
  #define i_valraw i_val
  #define i_valto c_default_toraw
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif
#if !defined i_eq && defined i_cmp
  #define i_eq !i_cmp
#elif !defined i_eq
  #define i_eq c_default_eq
#endif
#ifndef i_cmp
  #define i_cmp c_default_cmp
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
#undef i_eq
#undef i_hash
#undef i_from
#undef i_drop

#undef i_val
#undef i_val_str
#undef i_val_sptr
#undef i_val_bind
#undef i_valraw
#undef i_valfrom
#undef i_valto
#undef i_valdrop

#undef i_key
#undef i_key_str
#undef i_key_sptr
#undef i_key_bind
#undef i_keyraw
#undef i_keyfrom
#undef i_keyto
#undef i_keydrop

#undef _i_prefix
#undef _i_no_raw
#undef _i_no_keyraw
#undef _i_has_internal_clone
#undef _i_template
#endif
