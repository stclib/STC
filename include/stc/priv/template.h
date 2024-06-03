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
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define c_option(flag)  ((i_opt) & (flag))
  #define c_is_forward    (1<<0)
  #define c_no_atomic     (1<<1)
  #define c_no_clone      (1<<2)
  #define c_no_hash       (1<<4)
  #define c_use_cmp       (1<<5)
  #define c_use_eq        (1<<6)
  #define c_more          (1<<7)

  #define _c_MEMB(name) c_JOIN(i_type, name)
  #define _c_DEFTYPES(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  #define _m_value _c_MEMB(_value)
  #define _m_key _c_MEMB(_key)
  #define _m_mapped _c_MEMB(_mapped)
  #define _m_rmapped _c_MEMB(_rmapped)
  #define _m_raw _c_MEMB(_raw)
  #define _m_keyraw _c_MEMB(_keyraw)
  #define _m_iter _c_MEMB(_iter)
  #define _m_result _c_MEMB(_result)
  #define _m_node _c_MEMB(_node)
#endif

#if defined i_key2
  #define i_key c_SELECT(_c_SEL21, i_key2)
  #define i_keyraw c_SELECT(_c_SEL22, i_key2)
#endif
#if defined i_val2
  #define i_val c_SELECT(_c_SEL21, i_val2)
  #define i_valraw c_SELECT(_c_SEL22, i_val2)
#endif
#if defined i_keyclass2
  #define i_keyclass c_SELECT(_c_SEL21, i_keyclass2)
  #define i_keyraw c_SELECT(_c_SEL22, i_keyclass2)
#endif
#if defined i_valclass2
  #define i_valclass c_SELECT(_c_SEL21, i_valclass2)
  #define i_valraw c_SELECT(_c_SEL22, i_valclass2)
#endif
#if defined i_key_arc
  #define i_key_arcbox i_key_arc
#elif defined i_key_box
  #define i_key_arcbox i_key_box
#endif
#if defined i_val_arc
  #define i_val_arcbox i_val_arc
#elif defined i_val_box
  #define i_val_arcbox i_val_box
#endif

#if defined i_TYPE && defined _i_is_map
  #define i_type c_SELECT(_c_SEL31, i_TYPE)
  #define i_key c_SELECT(_c_SEL32, i_TYPE)
  #define i_val c_SELECT(_c_SEL33, i_TYPE)
#elif defined i_TYPE
  #define i_type c_SELECT(_c_SEL21, i_TYPE)
  #define i_key c_SELECT(_c_SEL22, i_TYPE)
#endif
#ifndef i_type
  #define i_type c_JOIN(_i_prefix, i_tag)
#endif

#if defined i_keyboxed || defined i_valboxed
  #error "i_keyboxed, i_valboxed is not supported. " \
         "Use: i_key_box, i_val_box, i_key_arc, i_val_arc."
#endif

#if !(defined i_key || defined i_key_str || \
      defined i_keyclass || defined i_key_arcbox)
  #if defined _i_is_map
    #error "i_key* must be defined for maps"
  #endif

  #if defined i_val_str
    #define i_key_str i_val_str
  #endif
  #if defined i_val_arcbox
    #define i_key_arcbox i_val_arcbox
  #endif
  #if defined i_valclass
    #define i_keyclass i_valclass
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

#define i_no_emplace

#if c_option(c_is_forward)
  #define i_is_forward
#endif
#if c_option(c_no_hash)
  #define i_no_hash
#endif
#if c_option(c_use_cmp)
  #define i_use_cmp
#endif
#if c_option(c_use_eq)
  #define i_use_eq
#endif
#if c_option(c_no_clone) || defined _i_is_arc
  #define i_no_clone
#endif
#if c_option(c_more)
  #define i_more
#endif

// Handle predefined element-types with lookup convertion types:
// cstr(const char*), cstr(csview), arc_T(T) / box_T(T)
#if defined i_key_str
  #define i_keyclass cstr
  #define i_rawclass ccharptr
  #define i_use_cmp
  #ifndef i_tag
    #define i_tag str
  #endif
#elif defined i_key_arcbox
  #define i_keyclass i_key_arcbox
  #define i_rawclass c_JOIN(i_key_arcbox, _raw)
#endif

// Check for i_keyclass and i_rawclass, and fill in missing defs:
// Element "class" type with possible assoc. convertion type and "member" functions.
#if defined i_rawclass
  #define i_keyraw i_rawclass
#elif defined i_keyclass && !defined i_keyraw
  #define i_rawclass i_key
#endif

// Bind to i_key "class members": _clone, _drop, _from and _toraw (when conditions are met).
#if defined i_keyclass
  #define i_key i_keyclass
  #if !defined i_keyclone && !defined i_no_clone
    #define i_keyclone c_JOIN(i_key, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_JOIN(i_key, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_JOIN(i_key, _from)
  #elif !defined i_keyfrom && !defined i_no_clone
    #define i_keyfrom c_JOIN(i_key, _clone)
  #endif
  #if !defined i_keyto && defined i_keyraw
    #define i_keyto c_JOIN(i_key, _toraw)
  #endif
#endif

// Define when container has support for sorting (cmp) and linear search (eq)
#if defined i_use_cmp || defined i_cmp || defined i_less
  #define _i_has_cmp
#endif
#if defined i_use_cmp || defined i_cmp || defined i_use_eq || defined i_eq
  #define _i_has_eq
#endif

// Bind to i_keyraw "class members": _cmp, _eq and _hash (when conditions are met).
#if defined i_rawclass
  #if !(defined i_cmp || defined i_less) && (defined i_use_cmp || defined _i_sorted || defined _i_ispque)
    #define i_cmp c_JOIN(i_keyraw, _cmp)
  #endif
  #if !defined i_eq && (defined i_use_eq || defined i_hash || defined _i_is_hash)
    #define i_eq c_JOIN(i_keyraw, _eq)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_keyraw, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "Both i_keyraw/i_valraw and i_keyto/i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
//#elif defined i_keyraw && defined _i_is_hash && !(defined i_hash && (defined _i_has_cmp || defined i_eq))
//  #error "For hmap/hset, both i_hash and i_eq (or i_cmp) must be defined when i_keyraw is defined."
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For smap/sset/pque, i_cmp or i_less must be defined when i_keyraw is defined."
#endif

// Fill in missing i_eq, i_less, i_cmp functions with defaults.
#if !defined i_eq && defined i_cmp
  #define i_eq(x, y) (i_cmp(x, y)) == 0
#elif !defined i_eq // && !defined i_keyraw
  #define i_eq(x, y) *x == *y // works for integral types
#endif
#if !defined i_less && defined i_cmp
  #define i_less(x, y) (i_cmp(x, y)) < 0
#elif !defined i_less // && !defined i_keyraw
  #define i_less(x, y) *x < *y // works for integral types
#endif
#if !defined i_cmp && defined i_less
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif
#if !(defined i_hash || defined i_no_hash)
  #define i_hash c_default_hash
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#if !defined i_keyfrom && defined i_keyclone && !defined i_keyraw
  #define i_keyfrom i_keyclone
#elif !defined i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #undef i_no_emplace
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
  #define i_keyto c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif

#if defined _i_is_map // ---- process hmap/smap value i_val, ... ----

#ifdef i_val_str
  #define i_valclass cstr
  #define i_valraw const char*
#elif defined i_val_arcbox
  #define i_valclass i_val_arcbox
  #define i_valraw c_JOIN(i_val_arcbox, _raw)
#endif

#ifdef i_valclass
  #define i_val i_valclass
  #if !defined i_valclone && !defined i_no_clone
    #define i_valclone c_JOIN(i_val, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_JOIN(i_val, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_JOIN(i_val, _from)
  #elif !defined i_valfrom && !defined i_no_clone
    #define i_valfrom c_JOIN(i_val, _clone)
  #endif
  #if !defined i_valto && defined i_valraw
    #define i_valto c_JOIN(i_val, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw ^ defined i_valto
  #error "Both i_valraw and i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#if !defined i_valfrom && defined i_valclone && !defined i_valraw
  #define i_valfrom i_valclone
#elif !defined i_valfrom
  #define i_valfrom c_default_clone
#else
  #undef i_no_emplace
#endif
#ifndef i_valraw
  #define i_valraw i_val
  #define i_valto c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif

#endif // !_i_is_map

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#endif // STC_TEMPLATE_H_INCLUDED
