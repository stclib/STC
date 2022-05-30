#ifndef utf8tabs_included
#define utf8tabs_included

#include <stdint.h>
struct CaseFold { uint16_t c0, c1, m1; } ;

extern struct CaseFold casefold[188];
extern uint8_t cfold_low[188];

#endif
