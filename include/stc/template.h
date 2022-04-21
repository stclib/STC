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
  #define _cx_self c_paste(_i_prefix, i_tag)
  #define _cx_memb(name) c_paste(_cx_self, name)
  #define _cx_deftypes(macro, SELF, ...) c_expand(macro(SELF, __VA_ARGS__))
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

#ifdef i_type
  #define i_tag i_type
  #undef _i_prefix
  #define _i_prefix
#endif

#ifndef i_size
  #define i_size uint32_t
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
#elif defined i_key_arcbox
  #define i_key_bind i_key_arcbox
  #define i_keyraw c_paste(i_key_arcbox, _value)
  #define i_keyclone c_paste(i_key_arcbox, _clone)
  #define _i_no_emplace
#endif

#ifdef i_key_bind
  #define i_key i_key_bind
  #ifndef i_keyraw
    #ifndef i_keyfrom
      #define i_keyfrom c_paste(i_key, _clone)
    #endif
  #else
    #ifndef i_keyfrom
      #define i_keyfrom c_paste(i_key, _from)
    #endif
    #ifndef i_keyto
      #define i_keyto c_paste(i_key, _toraw)
    #endif
  #endif
  #ifndef i_cmp
    #define i_cmp c_paste(i_keyraw, _cmp)
  #endif
  #ifndef i_eq
    #define i_eq c_paste(i_keyraw, _eq)
  #endif
  #ifndef i_hash
    #define i_hash c_paste(i_keyraw, _hash)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_paste(i_key, _drop)
  #endif
#endif

#if defined i_keyraw && !defined i_keyfrom
  #error "if i_keyraw is defined, i_keyfrom (and normally i_keyto) must be defined"
#elif defined i_drop
  #error "i_drop not supported. Define i_keydrop/i_valdrop instead."
#elif defined i_from
  #error "i_from not supported. Define i_keyfrom/i_valfrom instead."
#endif

#ifdef i_val_str
  #define i_val_bind cstr
  #define i_valraw crawstr
  #if !defined i_tag && !defined i_key
    #define i_tag str
  #endif
#elif defined i_val_arcbox
  #define i_val_bind i_val_arcbox
  #define i_valraw c_paste(i_val_arcbox, _value)
  #define i_valclone c_paste(i_val_arcbox, _clone)
  #define _i_no_emplace
#endif

#ifdef i_val_bind
  #define i_val i_val_bind
  #ifndef i_valraw
    #ifndef i_valfrom
      #define i_valfrom c_paste(i_val, _clone)
    #endif
  #else
    #ifndef i_valfrom
      #define i_valfrom c_paste(i_val, _from)
    #endif
    #ifndef i_valto
      #define i_valto c_paste(i_val, _toraw)
    #endif
  #endif
  #if !defined i_cmp && !defined i_key
    #define i_cmp c_paste(i_valraw, _cmp)
  #endif
  #if !defined i_hash && c_option(c_hash)
    #define i_hash c_paste(i_val, _hash)
  #endif
  #if !defined i_eq && c_option(c_eq)
    #define i_eq c_paste(i_val, _eq)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_paste(i_val, _drop)
  #endif
#endif

#if defined i_valraw && !defined i_valfrom
  #error "if i_valraw is defined, i_valfrom (and normally i_valto) must be defined"
#endif

#if !defined i_keyraw && !defined i_valraw
  #define _i_no_emplace
#endif

/* Copy i_val* macros to i_key* if _i_isset */
#if defined _i_isset && defined i_val
  #if !defined i_key
    #define i_key i_val
  #endif
  #if defined i_valraw && !defined i_keyraw
    #define i_keyraw i_valraw
  #endif
  #if defined i_valclone && !defined i_keyclone
    #define i_keyclone i_valclone
  #endif
  #if defined i_valfrom && !defined i_keyfrom
    #define i_keyfrom i_valfrom
  #endif
  #if defined i_valto && !defined i_keyto
    #define i_keyto i_valto
  #endif
  #if defined i_valdrop && !defined i_keydrop
    #define i_keydrop i_valdrop
  #endif
#endif

#ifdef i_key
  #if defined _i_isset && !defined i_val
    #define i_val i_key
  #endif
  #ifndef i_tag
    #define i_tag i_key  
  #endif
  #if !defined i_keyfrom && defined i_keydrop
    #define _i_no_clone
  #endif
  #if !defined i_hash && (defined i_keyfrom || defined i_cmp || defined i_eq)
    #define _i_no_hash 1
  #endif
  #if !defined i_cmp && !defined i_eq && defined i_hash
    #define _i_no_hash 2
  #endif
  #ifndef i_keyfrom
    #define i_keyfrom c_default_from
  #endif
  #ifndef i_keyraw
    #define i_keyraw i_key
  #endif
  #ifndef i_keyto
    #define i_keyto c_default_toraw
  #endif
  #ifndef i_keyclone
    #define i_keyclone(key) i_keyfrom((i_keyto((&(key)))))
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_default_drop
  #endif
#endif

#ifndef i_tag
  #define i_tag i_val
#endif
#if (!defined i_valfrom && defined i_valdrop) || c_option(c_no_clone)
  #define _i_no_clone
#endif
#ifndef i_valfrom
  #define i_valfrom c_default_from
#endif
#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valto
  #define i_valto c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone(val) i_valfrom((i_valto((&(val)))))
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif
#if !defined i_eq && defined i_cmp
  #define i_eq(x, y) !(i_cmp(x, y))
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
#undef i_size

#undef i_val
#undef i_val_str
#undef i_val_arcbox
#undef i_val_bind
#undef i_valraw
#undef i_valfrom
#undef i_valto
#undef i_valdrop
#undef i_valclone

#undef i_key
#undef i_key_str
#undef i_key_arcbox
#undef i_key_bind
#undef i_keyraw
#undef i_keyfrom
#undef i_keyto
#undef i_keydrop
#undef i_keyclone

#undef _i_prefix
#undef _i_no_clone
#undef _i_no_emplace
#undef _i_no_hash
#undef _i_template
#endif
