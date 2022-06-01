#define STC_EXTERN  // implement common extern, non-templated functions, e.g. _clist_mergesort().
#define STC_HEADER  // don't implement clist_int itself, just dummy declare it.

#define i_val int
#include "../include/stc/clist.h"

#define STC_IMPLEMENT // implement the following.

#include "../include/stc/cstr.h"
#include "../include/stc/csview.h"
#include "../include/stc/cbits.h"
#include "../include/stc/crandom.h"
