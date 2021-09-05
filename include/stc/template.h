#ifndef STC_TEMPLATE_INCLUDED
#define STC_TEMPLATE_INCLUDED

#if defined I_TAG
  #define i_TAG I_TAG
  #define i_IMP
#endif
#if defined f_TAG
  #define i_TAG f_TAG
  #define i_FWD
#endif
#if defined F_TAG
  #define i_TAG F_TAG
  #define i_FWD
  #define i_IMP
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

#if !defined i_TAG && (defined i_KEY_str || defined i_VAL_str)
  #define i_TAG str
#endif
#if !defined i_TAG && defined i_KEY
  #define i_TAG i_KEY
#elif !defined i_TAG && defined i_VAL
  #define i_TAG i_VAL
#endif

#define Self c_PASTE3(i_CNT, _, i_TAG)
#define cx_memb(name) c_PASTE(Self, name)

#define cx_value_t cx_memb(_value_t)
#define cx_rawvalue_t cx_memb(_rawvalue_t)
#define cx_iter_t cx_memb(_iter_t)

#if defined i_VALTO ^ defined i_VALRAW
  #error i_VALTO and i_VALRAW must both be defined
#endif
#if defined i_KEYTO ^ defined i_KEYRAW
  #error i_KEYTO and i_KEYRAW must both be defined
#endif

#if defined i_KEY
  #define cx_key_t cx_memb(_key_t)
  #define cx_rawkey_t cx_memb(_rawkey_t)
  #define cx_mapped_t cx_memb(_mapped_t)
  #define cx_rawmapped_t cx_memb(_rawmapped_t)
  #define cx_result_t cx_memb(_result_t)

  #if !defined i_KEYFROM && defined i_KEYDEL
    #define i_KEYFROM c_no_clone
  #elif !defined i_KEYFROM
    #define i_KEYFROM c_default_fromraw
  #endif
  #ifndef i_KEYRAW
    #define i_KEYRAW i_KEY
    #define i_KEYTO c_default_toraw
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
#ifndef i_EQU
  #define i_EQU c_default_equals
#endif
#ifndef i_HASH
  #define i_HASH c_default_hash
#endif
#ifndef i_CMP
  #define i_CMP c_default_compare
#endif

// typedef container types defined in forward.h
#define cx_deftypes(macro, Self, ...) macro(Self, __VA_ARGS__)
// criteria for implementation
#if !defined i_IMP && (defined STC_IMPLEMENTATION || !defined STC_HEADER)
#define i_IMP
#endif

#else // -------------------------------------------------------

#undef i_CNT
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

#undef Self
#undef cx_memb
#undef cx_deftypes
#undef cx_value_t
#undef cx_rawvalue_t
#undef cx_iter_t
#undef cx_key_t
#undef cx_rawkey_t
#undef cx_mapped_t
#undef cx_rawmapped_t
#undef cx_result_t

#undef STC_TEMPLATE_INCLUDED
#endif
