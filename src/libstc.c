#include <stddef.h>
#if 1
#define i_import
#include "../include/stc/utf8.h"
#define i_implement
#include "../include/stc/cstr.h"
#define i_implement
#include "../include/stc/cregex.h"
#define i_implement
#include "../include/stc/csview.h"
#else
#define i_import
#include "../include/stc/cregex.h"
#define i_implement
#include "../include/stc/csview.h"
#endif
