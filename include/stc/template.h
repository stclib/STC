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
#endif

#ifdef i_type
  #define i_tag i_type
  #undef _i_prefix
  #define _i_prefix
#endif

#ifndef i_size
  #define i_size uint32_t
#endif

#if !(defined i_key || defined i_key_str || defined i_key_ssv || \
      defined i_key_bind || defined i_key_arcbox)
  #define _i_key_from_val
  #if defined _i_ismap
    #error "i_key* must be defined for maps."
  #endif

  #if defined i_val_str
    #define i_key_str i_val_str
  #endif
  #if defined i_val_ssv
    #define i_key_ssv i_val_ssv
  #endif  
  #if defined i_val_arcbox
    #define i_key_arcbox i_val_arcbox
  #endif
  #if defined i_val_bind
    #define i_key_bind i_val_bind
  #endif
  #if defined i_val
    #define i_key i_val
  #endif
  #if defined i_valraw
    #define i_keyraw i_valraw
  #endif
  #if defined i_valclone
    #define i_keyclone i_valclone
  #endif
  #if defined i_valfrom
    #define i_keyfrom i_valfrom
  #endif
  #if defined i_valto
    #define i_keyto i_valto
  #endif
  #if defined i_valdrop
    #define i_keydrop i_valdrop
  #endif
#endif

#if defined i_key_str
  #define i_key_bind cstr
  #define i_keyraw crawstr
  #define i_keyfrom cstr_from
  #ifndef i_tag
    #define i_tag str
  #endif
#elif defined i_key_ssv
  #define i_key_bind cstr
  #define i_keyraw csview
  #define i_keyfrom cstr_from_sv
  #define i_keyto cstr_sv
  #define i_eq csview_eq
  #ifndef i_tag
    #define i_tag ssv
  #endif
#elif defined i_key_arcbox
  #define i_key_bind i_key_arcbox
  #define i_keyraw c_paste(i_key_arcbox, _value)
  #define i_keyto c_paste(i_key, _toval)
  #define i_eq c_paste(i_key_arcbox, _value_eq)
#endif

#ifdef i_key_bind
  #define i_key i_key_bind
  #ifndef i_keyclone
    #define i_keyclone c_paste(i_key, _clone)
  #endif
  #if !defined i_keyto && defined i_keyraw
    #define i_keyto c_paste(i_key, _toraw)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_paste(i_key, _drop)
  #endif
  #ifndef i_cmp
    #define i_cmp c_paste(i_keyraw, _cmp)
  #endif
  #if !defined i_hash
    #define i_hash c_paste(i_keyraw, _hash)
  #endif
#endif

#if !defined i_key
  #error "no i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "both i_keyraw and i_keyto must be defined, if any"
#elif defined i_keyfrom && !defined i_keyraw
  #error "i_keyfrom defined without i_keyraw"
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#if c_option(c_no_clone) || (!defined i_keyclone && (defined i_keydrop || defined i_keyraw))
  #define _i_no_clone
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #define _i_has_from
#endif
#ifndef i_keyto
  #define i_keyto c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif
#ifdef i_less
  #define i_cmp(x, y) c_less_cmp(i_less, x, y)
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

#if defined _i_ismap // ---- process cmap/csmap value i_val, ... ----

#ifdef i_val_str
  #define i_val_bind cstr
  #define i_valraw crawstr
  #define i_valfrom cstr_from
#elif defined i_val_ssv
  #define i_val_bind cstr
  #define i_valraw csview
  #define i_valfrom cstr_from_sv
  #define i_valto cstr_sv
#elif defined i_val_arcbox
  #define i_val_bind i_val_arcbox
  #define i_valraw c_paste(i_val_arcbox, _value)
  #define i_valto c_paste(i_val, _toval)  
#endif

#ifdef i_val_bind
  #define i_val i_val_bind
  #ifndef i_valclone
    #define i_valclone c_paste(i_val, _clone)
  #endif
  #if !defined i_valto && defined i_valraw
    #define i_valto c_paste(i_val, _toraw)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_paste(i_val, _drop)
  #endif
#endif

#if defined i_valraw ^ defined i_valto
  #error "both i_valto and i_valraw must be defined, if any"
#elif defined i_valfrom && !defined i_valraw
  #error "i_valfrom defined without i_valraw"
#endif

#if !defined i_valclone && (defined i_valdrop || defined i_valraw)
  #define _i_no_clone
#endif
#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valfrom
  #define i_valfrom c_default_clone
#else
  #define _i_has_from
#endif
#ifndef i_valto
  #define i_valto c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif

#endif // !_i_ismap

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#ifndef _i_has_from
  #define _i_no_emplace
#endif

#else // ============================================================

#undef i_type
#undef i_tag
#undef i_imp
#undef i_opt
#undef i_less
#undef i_cmp
#undef i_eq
#undef i_hash
#undef i_size

#undef i_val
#undef i_val_str
#undef i_val_ssv
#undef i_val_arcbox
#undef i_val_bind
#undef i_valraw
#undef i_valclone
#undef i_valfrom
#undef i_valto
#undef i_valdrop

#undef i_key
#undef i_key_str
#undef i_key_ssv
#undef i_key_arcbox
#undef i_key_bind
#undef i_keyraw
#undef i_keyclone
#undef i_keyfrom
#undef i_keyto
#undef i_keydrop

#undef i_header
#undef i_implement
#undef i_static
#undef i_extern

#undef _i_prefix
#undef _i_has_from
#undef _i_key_from_val
#undef _i_no_clone
#undef _i_no_emplace
#undef _i_no_hash
#undef _i_template
#endif
