#define STC_EXTERN    // implement common extern, non-templated functions and dependencies.

#define i_val int
#define i_header      // don't implement clist_int itself, just dummy declare it.
#include "../include/stc/clist.h"

#define STC_IMPLEMENT // implement the following.

#include "../include/stc/cregex.h"
#include "../include/stc/csview.h"
//#include "../include/stc/crandom.h"
