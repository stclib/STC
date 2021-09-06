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
#ifndef i_TEMPLATE
#define i_TEMPLATE

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define cx_memb(name) c_PASTE(Self, name)
  #define Self c_PASTE3(i_MODULE, _, i_TAG)
  // typedef container types defined in forward.h
  #define cx_deftypes(macro, SELF, ...) macro(SELF, __VA_ARGS__)
  
  #define cx_value_t cx_memb(_value_t)
  #define cx_key_t cx_memb(_key_t)
  #define cx_mapped_t cx_memb(_mapped_t)
  #define cx_rawvalue_t cx_memb(_rawvalue_t)
  #define cx_rawkey_t cx_memb(_rawkey_t)
  #define cx_rawmapped_t cx_memb(_rawmapped_t)
  #define cx_iter_t cx_memb(_iter_t)
  #define cx_result_t cx_memb(_result_t)
  #define cx_node_t cx_memb(_node_t)
#endif

#if defined f_TAG
  #define i_TAG f_TAG
  #define i_FWD
#endif

#if defined i_KEY_str
  #define i_KEY     cstr
  #define i_CMP     c_rawstr_compare
  #define i_EQU     c_rawstr_equals
  #define i_HASH    c_rawstr_hash
  #define i_KEYDEL  cstr_del
  #define i_KEYFROM cstr_from
  #define i_KEYTO   cstr_str
  #define i_KEYRAW  const char*
#endif
#if defined i_VAL_str
  #define i_VAL     cstr
  #ifndef i_KEY
  #define i_CMP     c_rawstr_compare
  #endif
  #define i_VALDEL  cstr_del
  #define i_VALFROM cstr_from
  #define i_VALTO   cstr_str
  #define i_VALRAW  const char*
#endif
#if defined i_KEY && !defined i_VAL
  #define i_VAL i_KEY
#endif
#if !defined i_TAG && defined i_KEY_str
  #define i_TAG str
#elif !defined i_TAG && defined i_KEY
  #define i_TAG i_KEY
#elif !defined i_TAG && defined i_VAL_str
  #define i_TAG str
#elif !defined i_TAG && defined i_VAL
  #define i_TAG i_VAL
#endif

#if (defined i_VALTO ^ defined i_VALRAW) || (defined i_VALRAW && !defined i_VALFROM)
  #error if i_VALRAW defined, both i_VALFROM and i_VALTO must be defined
#endif
#if (defined i_KEYTO ^ defined i_KEYRAW) || (defined i_KEYRAW && !defined i_KEYFROM)
  #error if i_KEYRAW defined, both i_KEYFROM and i_KEYTO must be defined
#endif

#if defined i_KEY
  #if !defined i_KEYFROM && defined i_KEYDEL
    #define i_KEYFROM c_no_clone
  #elif !defined i_KEYFROM
    #define i_KEYFROM c_default_fromraw
  #endif
  #ifndef i_KEYRAW
    #define i_KEYRAW i_KEY
    #define i_KEYTO c_default_toraw
  #endif
  #if !defined i_EQU && defined i_CMP
    #define i_EQU !i_CMP
  #elif !defined i_EQU
    #define i_EQU c_default_equals
  #endif
  #ifndef i_HASH
    #define i_HASH c_default_hash
  #endif
#endif

#if !defined i_VALFROM && defined i_VALDEL
  #define i_VALFROM c_no_clone
#elif !defined i_VALFROM
  #define i_VALFROM c_default_fromraw
#endif
#ifndef i_VALRAW
  #define i_VALRAW i_VAL
  #define i_VALTO c_default_toraw
#endif
#ifndef i_KEYDEL
  #define i_KEYDEL c_default_del
#endif
#ifndef i_VALDEL
  #define i_VALDEL c_default_del
#endif
#ifndef i_CMP
  #define i_CMP c_default_compare
#endif

#else // -------------------------------------------------------

#undef i_MODULE
#undef i_TAG
#undef f_TAG
#undef i_IMP
#undef i_FWD
#undef i_CMP
#undef i_EQU
#undef i_HASH
#undef i_VAL
#undef i_VAL_str
#undef i_VALDEL
#undef i_VALFROM
#undef i_VALTO
#undef i_VALRAW
#undef i_KEY
#undef i_KEY_str
#undef i_KEYDEL
#undef i_KEYFROM
#undef i_KEYTO
#undef i_KEYRAW

#undef i_TEMPLATE
#endif
