#if __STDC_VERSION__ >= 201112L
  #define STC_IMPLEMENT_CSERDE
  #include "../include/stc/c11/serialize.h" // also implements cstream.
  #define STC_IMPLEMENT_FMT
  #include "../include/stc/c11/fmt.h"
#endif
#define STC_IMPLEMENT_CSTREAM
#include "../include/stc/sys/cstream.h" // also implements cstream.
