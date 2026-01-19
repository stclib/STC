/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
// IWYU pragma: private
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED

  #define _c_MEMB(name) c_JOIN(Self, name)
  #define _c_DEFTYPES(macro, SELF, ...) macro(SELF, __VA_ARGS__)
  #define _m_value _c_MEMB(_value)
  #define _m_key _c_MEMB(_key)
  #define _m_mapped _c_MEMB(_mapped)
  #define _m_rmapped _c_MEMB(_rmapped)
  #define _m_raw _c_MEMB(_raw)
  #define _m_keyraw _c_MEMB(_keyraw)
  #define _m_iter _c_MEMB(_iter)
  #define _m_result _c_MEMB(_result)
  #define _m_node _c_MEMB(_node)

  #define c_OPTION(flag)  ((i_opt) & (flag))
  #define c_declared      (1<<0)
  #define c_no_atomic     (1<<1)
  #define c_use_arc2      (1<<2)
  #define c_no_clone      (1<<3)
  #define c_use_cmp       (1<<5)
  #define c_use_eq        (1<<6)
  #define c_use_comp      (c_use_cmp | c_use_eq)
  #define c_comp_key      (1<<7)
  #define c_class_key     (1<<8)
  #define c_class_val     (1<<9)
  #define c_pro_key       (1<<10)
  #define c_pro_val       (1<<11)

  #define c_keycomp  c_comp_key  // [deprecated]
  #define c_cmpclass c_comp_key  // [deprecated]
  #define c_keyclass c_class_key // [deprecated]
  #define c_valclass c_class_val // [deprecated]
  #define c_keypro   c_pro_key   // [deprecated]
  #define c_valpro   c_pro_val   // [deprecated]
#endif
#ifdef i_keycomp                 // [deprecated]
  #define i_comp_key i_keycomp
#elif defined i_keyclass         // [deprecated]
  #define i_class_key i_keyclass
#elif defined i_keypro           // [deprecated]
  #define i_pro_key i_keypro
#endif
#if defined i_valclass           // [deprecated]
  #define i_class_val i_valclass
#elif defined i_keypro           // [deprecated]
  #define i_pro_val i_valpro
#endif

#if defined T && !defined i_type
  #define i_type T
#endif
#if defined i_type && c_NUMARGS(i_type) > 1
  #define Self c_GETARG(1, i_type)
  #define i_key c_GETARG(2, i_type)
  #ifdef _i_is_map
    #define i_val c_GETARG(3, i_type)
    #if c_NUMARGS(i_type) == 4
      #define i_opt c_GETARG(4, i_type)+0
    #endif
  #elif c_NUMARGS(i_type) >= 3
    #define i_opt c_GETARG(3, i_type)+0
  #endif
#elif !defined Self && defined i_type
  #define Self i_type
#elif !defined Self
  #define Self c_JOIN(_i_prefix, i_tag)
#endif

#if defined i_aux && c_NUMARGS(i_aux) == 2
  // shorthand for defining i_aux AND i_allocator as a one-liner combo.
  #define _i_aux_alloc
  #define _i_aux_def c_GETARG(1, i_aux) aux;
  #undef i_allocator // override:
  #define i_allocator c_GETARG(2, i_aux)
#elif defined i_aux
  #define _i_aux_def i_aux aux;
#else
  #define _i_aux_def
#endif

#if c_OPTION(c_declared)
  #define i_declared
#endif
#if c_OPTION(c_use_cmp)
  #define i_use_cmp
#endif
#if c_OPTION(c_use_eq)
  #define i_use_eq
#endif
#if c_OPTION(c_no_clone) || defined _i_is_arc
  #define i_no_clone
#endif
#if c_OPTION(c_class_key)
  #define i_class_key i_key
#endif
#if c_OPTION(c_class_val)
  #define i_class_val i_val
#endif
#if c_OPTION(c_comp_key)
  #define i_comp_key i_key
  #define i_use_cmp
  #define i_use_eq
#endif
#if c_OPTION(c_pro_key)
  #define i_pro_key i_key
#endif
#if c_OPTION(c_pro_val)
  #define i_pro_val i_val
#endif

#if defined i_pro_key
  #define i_class_key i_pro_key
  #define i_comp_key c_JOIN(i_pro_key, _raw)
#endif

#if defined i_comp_key
  #define i_keyraw i_comp_key
#elif defined i_class_key && !defined i_keyraw
  // Also bind comparisons functions when c_class_key is specified.
  #define i_comp_key i_key
#elif defined i_keyraw && !defined i_keyfrom
  // Define _i_no_put when i_keyfrom is not explicitly defined and i_keyraw is.
  // In this case, i_keytoraw needs to be defined (may be done later in this file).
  #define _i_no_put
#endif

// Bind to i_key "class members": _clone, _drop, _from and _toraw (when conditions are met).
#if defined i_class_key
  #ifndef i_key
    #define i_key i_class_key
  #endif
  #if !defined i_keyclone && !defined i_no_clone
    #define i_keyclone c_JOIN(i_class_key, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_JOIN(i_class_key, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_JOIN(i_class_key, _from)
  #endif
  #if !defined i_keytoraw && defined i_keyraw
    #define i_keytoraw c_JOIN(i_class_key, _toraw)
  #endif
#endif

// Define when container has support for sorting (cmp) and linear search (eq)
#if defined i_use_cmp || defined i_cmp || defined i_less || defined _i_sorted
  #define _i_has_cmp
#endif
#if defined i_use_eq || defined i_eq || defined i_hash || defined _i_hasher
  #define _i_has_eq
#endif

// Bind to i_comp_key "class members": _cmp, _eq and _hash (when conditions are met).
#if defined i_comp_key
  #if !(defined i_cmp || defined i_less) && defined _i_has_cmp
    #define i_cmp c_JOIN(i_comp_key, _cmp)
  #endif
  #if !defined i_eq && defined _i_has_eq
    #define i_eq c_JOIN(i_comp_key, _eq)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_comp_key, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key defined"
#elif defined i_keyraw && !(c_OPTION(c_comp_key) || defined i_keytoraw)
  #error "If i_comp_key / i_keyraw is defined, i_keytoraw must be defined too"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone and i_keydrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Use i_keyfrom/i_keydrop"
#elif defined i_keyto || defined i_valto
  #error i_keyto / i_valto not supported. Use i_keytoraw / i_valtoraw
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For smap / sset / pqueue, i_cmp or i_less must be defined when i_keyraw is defined."
#endif

// Fill in missing i_eq, i_less, i_cmp functions with defaults.
#if !defined i_eq
  #define i_eq(x, y) *x == *y // works for integral types
  #define _i_has_default_eq
#endif
#if !defined i_less && defined i_cmp
  #define i_less(x, y) (i_cmp(x, y)) < 0
#elif !defined i_less
  #define i_less(x, y) *x < *y // works for integral types
#endif
#if !defined i_cmp && defined i_less
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif
#if !(defined i_hash || defined i_no_hash)
  #define i_hash c_default_hash
#endif

#define _i_no_emplace
#define _i_is_trivial

#ifndef i_tag
  #define i_tag i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #undef _i_no_emplace
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
#endif
#ifndef i_keytoraw
  #define i_keytoraw c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#else
  #undef _i_is_trivial
#endif

#if defined _i_is_map // ---- process hashmap/sortedmap value i_val, ... ----

#if defined i_pro_val
  #define i_class_val i_pro_val
  #define i_valraw c_JOIN(i_pro_val, _raw)
#endif

#ifdef i_class_val
  #ifndef i_val
    #define i_val i_class_val
  #endif
  #if !defined i_valclone && !defined i_no_clone
    #define i_valclone c_JOIN(i_class_val, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_JOIN(i_class_val, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_JOIN(i_class_val, _from)
  #endif
  #if !defined i_valtoraw && defined i_valraw
    #define i_valtoraw c_JOIN(i_class_val, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw && !defined i_valtoraw
  #error "If i_valraw is defined, i_valtoraw must be defined too"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#ifndef i_valfrom
  #define i_valfrom c_default_clone
  #ifdef i_valraw
    #define _i_no_put
  #endif
#else
  #undef _i_no_emplace
#endif
#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valtoraw
  #define i_valtoraw c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#else
  #undef _i_is_trivial
#endif

#endif // !_i_is_map

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#endif // STC_TEMPLATE_H_INCLUDED
