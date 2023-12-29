/*
This is a Unix port of the Plan 9 regular expression library, by Rob Pike.
Please send comments about the packaging to Russ Cox <rsc@swtch.com>.

Copyright © 2021 Plan 9 Foundation
Copyright © 2023 Tyge Løvset, for additions.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef STC_CREGEX_PRV_C_INCLUDED
#define STC_CREGEX_PRV_C_INCLUDED

#include <setjmp.h>
#include "utf8_prv.h"
#include "cstr_prv.h"

typedef uint32_t _Rune; /* Utf8 code point */
typedef int32_t _Token;
/* max character classes per program */
#define _NCLASS CREG_MAX_CLASSES
/* max subexpressions */
#define _NSUBEXP CREG_MAX_CAPTURES
/* max rune ranges per character class */
#define _NCCRUNE (_NSUBEXP * 2)

/*
 *    character class, each pair of rune's defines a range
 */
typedef struct
{
    _Rune *end;
    _Rune spans[_NCCRUNE];
} _Reclass;

/*
 *    Machine instructions
 */
typedef struct _Reinst
{
    _Token type;
    union {
        _Reclass *classp;        /* class pointer */
        _Rune    rune;           /* character */
        int     subid;           /* sub-expression id for TOK_RBRA and TOK_LBRA */
        struct _Reinst *right;   /* right child of TOK_OR */
    } r;
    union {    /* regexp relies on these two being in the same union */
        struct _Reinst *left;    /* left child of TOK_OR */
        struct _Reinst *next;    /* next instruction for TOK_CAT & TOK_LBRA */
    } l;
} _Reinst;

typedef struct {
    bool icase;
    bool dotall;
} _Reflags;

/*
 *    Reprogram definition
 */
typedef struct _Reprog
{
    _Reinst  *startinst;     /* start pc */
    _Reflags flags;
    int nsubids;
    intptr_t allocsize;
    _Reclass cclass[_NCLASS]; /* .data */
    _Reinst  firstinst[];    /* .text : originally 5 elements? */
} _Reprog;

/*
 *    Sub expression matches
 */
typedef csview _Resub;

/*
 *  substitution list
 */
typedef struct _Resublist
{
    _Resub m[_NSUBEXP];
} _Resublist;

/*
 * Actions and Tokens (_Reinst types)
 *
 *  0x800000-0x80FFFF: operators, value => precedence
 *  0x810000-0x81FFFF: TOK_RUNE and char classes.
 *  0x820000-0x82FFFF: tokens, i.e. operands for operators
 */
enum {
    TOK_MASK    = 0xFF00000,
    TOK_OPERATOR = 0x8000000,   /* Bitmask of all operators */
    TOK_START   = 0x8000001,    /* Start, used for marker on stack */
    TOK_RBRA    ,               /* Right bracket, ) */
    TOK_LBRA    ,               /* Left bracket, ( */
    TOK_OR      ,               /* Alternation, | */
    TOK_CAT     ,               /* Concatentation, implicit operator */
    TOK_STAR    ,               /* Closure, * */
    TOK_PLUS    ,               /* a+ == aa* */
    TOK_QUEST   ,               /* a? == a|nothing, i.e. 0 or 1 a's */
    TOK_RUNE    = 0x8100000,
    TOK_IRUNE   ,
    ASC_an      , ASC_AN,       /* alphanum */
    ASC_al      , ASC_AL,       /* alpha */
    ASC_as      , ASC_AS,       /* ascii */
    ASC_bl      , ASC_BL,       /* blank */
    ASC_ct      , ASC_CT,       /* ctrl */
    ASC_d       , ASC_D,        /* digit */
    ASC_s       , ASC_S,        /* space */
    ASC_w       , ASC_W,        /* word */
    ASC_gr      , ASC_GR,       /* graphic */
    ASC_pr      , ASC_PR,       /* print */
    ASC_pu      , ASC_PU,       /* punct */
    ASC_lo      , ASC_LO,       /* lower */
    ASC_up      , ASC_UP,       /* upper */
    ASC_xd      , ASC_XD,       /* hex */
    UTF_al      , UTF_AL,       /* utf8 alpha */
    UTF_an      , UTF_AN,       /* utf8 alphanumeric */
    UTF_bl      , UTF_BL,       /* utf8 blank */
    UTF_lc      , UTF_LC,       /* utf8 letter cased */
    UTF_ll      , UTF_LL,       /* utf8 letter lowercase */
    UTF_lu      , UTF_LU,       /* utf8 letter uppercase */
    UTF_sp      , UTF_SP,       /* utf8 space */
    UTF_wr      , UTF_WR,       /* utf8 word */
    UTF_GRP = 0x8150000,
    UTF_cc = UTF_GRP+2*U8G_Cc, UTF_CC, /* utf8 control char */
    UTF_lt = UTF_GRP+2*U8G_Lt, UTF_LT, /* utf8 letter titlecase */
    UTF_nd = UTF_GRP+2*U8G_Nd, UTF_ND, /* utf8 number decimal */
    UTF_nl = UTF_GRP+2*U8G_Nl, UTF_NL, /* utf8 number letter */
    UTF_pc = UTF_GRP+2*U8G_Pc, UTF_PC, /* utf8 punct connector */
    UTF_pd = UTF_GRP+2*U8G_Pd, UTF_PD, /* utf8 punct dash */
    UTF_pf = UTF_GRP+2*U8G_Pf, UTF_PF, /* utf8 punct final */
    UTF_pi = UTF_GRP+2*U8G_Pi, UTF_PI, /* utf8 punct initial */
    UTF_sc = UTF_GRP+2*U8G_Sc, UTF_SC, /* utf8 symbol currency */
    UTF_zl = UTF_GRP+2*U8G_Zl, UTF_ZL, /* utf8 separator line */
    UTF_zp = UTF_GRP+2*U8G_Zp, UTF_ZP, /* utf8 separator paragraph */
    UTF_zs = UTF_GRP+2*U8G_Zs, UTF_ZS, /* utf8 separator space */
    UTF_arabic = UTF_GRP+2*U8G_Arabic, UTF_ARABIC,
    UTF_cyrillic = UTF_GRP+2*U8G_Cyrillic, UTF_CYRILLIC,
    UTF_devanagari = UTF_GRP+2*U8G_Devanagari, UTF_DEVANAGARI,
    UTF_greek = UTF_GRP+2*U8G_Greek, UTF_GREEK,
    UTF_han = UTF_GRP+2*U8G_Han, UTF_HAN,
    UTF_latin = UTF_GRP+2*U8G_Latin, UTF_LATIN,
    TOK_ANY     = 0x8200000,    /* Any character except newline, . */
    TOK_ANYNL   ,               /* Any character including newline, . */
    TOK_NOP     ,               /* No operation, internal use only */
    TOK_BOL     , TOK_BOS,      /* Beginning of line / string, ^ */
    TOK_EOL     , TOK_EOS,      /* End of line / string, $ */
    TOK_EOZ     ,               /* End of line with optional NL */
    TOK_CCLASS  ,               /* Character class, [] */
    TOK_NCCLASS ,               /* Negated character class, [] */
    TOK_WBOUND  ,               /* Non-word boundary, not consuming meta char */
    TOK_NWBOUND ,               /* Word boundary, not consuming meta char */
    TOK_CASED   ,               /* (?-i) */
    TOK_ICASE   ,               /* (?i) */
    TOK_END     = 0x82FFFFF,    /* Terminate: match found */
};

/*
 *  _regexec execution lists
 */
#define _LISTSIZE    10
#define _BIGLISTSIZE (10*_LISTSIZE)

typedef struct _Relist
{
    _Reinst*    inst;       /* Reinstruction of the thread */
    _Resublist  se;         /* matched subexpressions in this thread */
} _Relist;

typedef struct _Reljunk
{
    _Relist*    relist[2];
    _Relist*    reliste[2];
    int         starttype;
    _Rune       startchar;
    const char* starts;
    const char* eol;
} _Reljunk;

/*
 * utf8 and _Rune code
 */

static int
chartorune(_Rune *rune, const char *s)
{
    utf8_decode_t ctx = {.state=0};
    const uint8_t *b = (const uint8_t*)s;
    do { utf8_decode(&ctx, *b++); } while (ctx.state);
    *rune = ctx.codep;
    return (int)((const char*)b - s);
}

static const char*
utfrune(const char *s, _Rune c)
{
    if (c < 128)        /* ascii */
        return strchr((char *)s, (int)c);
    int n;
    for (_Rune r = (uint32_t)*s; r; s += n, r = *(unsigned char*)s) {
        if (r < 128) { n = 1; continue; }
        n = chartorune(&r, s);
        if (r == c) return s;
    }
    return NULL;
}

static const char*
utfruneicase(const char *s, _Rune c)
{
    _Rune r = (uint32_t)*s;
    int n;
    if (c < 128) for (c = (_Rune)tolower((int)c); r; ++s, r = *(unsigned char*)s) {
        if (r < 128 && (_Rune)tolower((int)r) == c) return s;
    }
    else for (c = utf8_casefold(c); r; s += n, r = *(unsigned char*)s) {
        if (r < 128) { n = 1; continue; }
        n = chartorune(&r, s);
        if (utf8_casefold(r) == c) return s;
    }
    return NULL;
}

/************
 * regaux.c *
 ************/

/*
 *  save a new match in mp
 */
static void
_renewmatch(_Resub *mp, int ms, _Resublist *sp, int nsubids)
{
    if (mp==NULL || ms==0)
        return;
    if (mp[0].buf == NULL || sp->m[0].buf < mp[0].buf ||
       (sp->m[0].buf == mp[0].buf && sp->m[0].size > mp[0].size)) {
        for (int i=0; i<ms && i<=nsubids; i++)
            mp[i] = sp->m[i];
    }
}

/*
 * Note optimization in _renewthread:
 *   *lp must be pending when _renewthread called; if *l has been looked
 *   at already, the optimization is a bug.
 */
static _Relist*
_renewthread(_Relist *lp,  /* _relist to add to */
    _Reinst *ip,           /* instruction to add */
    int ms,
    _Resublist *sep)       /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sep->m[0].buf < p->se.m[0].buf) {
                if (ms > 1)
                    p->se = *sep;
                else
                    p->se.m[0] = sep->m[0];
            }
            return 0;
        }
    }
    p->inst = ip;
    if (ms > 1)
        p->se = *sep;
    else
        p->se.m[0] = sep->m[0];
    (++p)->inst = NULL;
    return p;
}

/*
 * same as renewthread, but called with
 * initial empty start pointer.
 */
static _Relist*
_renewemptythread(_Relist *lp,   /* _relist to add to */
    _Reinst *ip,                 /* instruction to add */
    int ms,
    const char *sp)             /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sp < p->se.m[0].buf) {
                if (ms > 1)
                    memset(&p->se, 0, sizeof(p->se));
                p->se.m[0].buf = sp;
            }
            return 0;
        }
    }
    p->inst = ip;
    if (ms > 1)
        memset(&p->se, 0, sizeof(p->se));
    p->se.m[0].buf = sp;
    (++p)->inst = NULL;
    return p;
}

/*
 * _Parser Information
 */
typedef struct _Node
{
    _Reinst*    first;
    _Reinst*    last;
} _Node;

#define _NSTACK 20
typedef struct _Parser
{
    const char* exprp;   /* pointer to next character in source expression */
    _Node andstack[_NSTACK];
    _Node* andp;
    _Token atorstack[_NSTACK];
    _Token* atorp;
    short subidstack[_NSTACK]; /* parallel to atorstack */
    short* subidp;
    short cursubid;      /* id of current subexpression */
    int error;
    _Reflags flags;
    int dot_type;
    int rune_type;
    bool litmode;
    bool lastwasand;     /* Last token was _operand */
    short nbra;
    short nclass;
    intptr_t instcap;
    _Rune yyrune;         /* last lex'd rune */
    _Reclass *yyclassp;   /* last lex'd class */
    _Reclass* classp;
    _Reinst* freep;
    jmp_buf regkaboom;
} _Parser;

/* predeclared crap */
static void _operator(_Parser *par, _Token type);
static void _pushand(_Parser *par, _Reinst *first, _Reinst *last);
static void _pushator(_Parser *par, _Token type);
static void _evaluntil(_Parser *par, _Token type);
static int  _bldcclass(_Parser *par);

static void
_rcerror(_Parser *par, cregex_result err)
{
    par->error = err;
    longjmp(par->regkaboom, 1);
}

static _Reinst*
_newinst(_Parser *par, _Token t)
{
    par->freep->type = t;
    par->freep->l.left = 0;
    par->freep->r.right = 0;
    return par->freep++;
}

static void
_operand(_Parser *par, _Token t)
{
    _Reinst *i;

    if (par->lastwasand)
        _operator(par, TOK_CAT);    /* catenate is implicit */
    i = _newinst(par, t);
    switch (t) {
    case TOK_CCLASS: case TOK_NCCLASS:
        i->r.classp = par->yyclassp; break;
    case TOK_RUNE:
        i->r.rune = par->yyrune; break;
    case TOK_IRUNE:
        i->r.rune = utf8_casefold(par->yyrune);
    }
    _pushand(par, i, i);
    par->lastwasand = true;
}

static void
_operator(_Parser *par, _Token t)
{
    if (t==TOK_RBRA && --par->nbra<0)
        _rcerror(par, CREG_UNMATCHEDRIGHTPARENTHESIS);
    if (t==TOK_LBRA) {
        if (++par->cursubid >= _NSUBEXP)
            _rcerror(par, CREG_TOOMANYSUBEXPRESSIONS);
        par->nbra++;
        if (par->lastwasand)
            _operator(par, TOK_CAT);
    } else
        _evaluntil(par, t);
    if (t != TOK_RBRA)
        _pushator(par, t);
    par->lastwasand = 0;
    if (t==TOK_STAR || t==TOK_QUEST || t==TOK_PLUS || t==TOK_RBRA)
        par->lastwasand = true;    /* these look like operands */
}

static void
_pushand(_Parser *par, _Reinst *f, _Reinst *l)
{
    if (par->andp >= &par->andstack[_NSTACK])
        _rcerror(par, CREG_OPERANDSTACKOVERFLOW);
    par->andp->first = f;
    par->andp->last = l;
    par->andp++;
}

static void
_pushator(_Parser *par, _Token t)
{
    if (par->atorp >= &par->atorstack[_NSTACK])
        _rcerror(par, CREG_OPERATORSTACKOVERFLOW);
    *par->atorp++ = t;
    *par->subidp++ = par->cursubid;
}

static _Node*
_popand(_Parser *par, _Token op)
{
    (void)op;
    _Reinst *inst;

    if (par->andp <= &par->andstack[0]) {
        _rcerror(par, CREG_MISSINGOPERAND);
        inst = _newinst(par, TOK_NOP);
        _pushand(par, inst, inst);
    }
    return --par->andp;
}

static _Token
_popator(_Parser *par)
{
    if (par->atorp <= &par->atorstack[0])
        _rcerror(par, CREG_OPERATORSTACKUNDERFLOW);
    --par->subidp;
    return *--par->atorp;
}


static void
_evaluntil(_Parser *par, _Token pri)
{
    _Node *op1, *op2;
    _Reinst *inst1, *inst2;

    while (pri==TOK_RBRA || par->atorp[-1]>=pri) {
        switch (_popator(par)) {
        default:
            _rcerror(par, CREG_UNKNOWNOPERATOR);
            break;
        case TOK_LBRA:        /* must have been TOK_RBRA */
            op1 = _popand(par, '(');
            inst2 = _newinst(par, TOK_RBRA);
            inst2->r.subid = *par->subidp;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, TOK_LBRA);
            inst1->r.subid = *par->subidp;
            inst1->l.next = op1->first;
            _pushand(par, inst1, inst2);
            return;
        case TOK_OR:
            op2 = _popand(par, '|');
            op1 = _popand(par, '|');
            inst2 = _newinst(par, TOK_NOP);
            op2->last->l.next = inst2;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, TOK_OR);
            inst1->r.right = op1->first;
            inst1->l.left = op2->first;
            _pushand(par, inst1, inst2);
            break;
        case TOK_CAT:
            op2 = _popand(par, 0);
            op1 = _popand(par, 0);
            op1->last->l.next = op2->first;
            _pushand(par, op1->first, op2->last);
            break;
        case TOK_STAR:
            op2 = _popand(par, '*');
            inst1 = _newinst(par, TOK_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, inst1, inst1);
            break;
        case TOK_PLUS:
            op2 = _popand(par, '+');
            inst1 = _newinst(par, TOK_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, op2->first, inst1);
            break;
        case TOK_QUEST:
            op2 = _popand(par, '?');
            inst1 = _newinst(par, TOK_OR);
            inst2 = _newinst(par, TOK_NOP);
            inst1->l.left = inst2;
            inst1->r.right = op2->first;
            op2->last->l.next = inst2;
            _pushand(par, inst1, inst2);
            break;
        }
    }
}


static _Reprog*
_optimize(_Parser *par, _Reprog *pp)
{
    _Reinst *inst, *target;
    _Reclass *cl;

    /*
     *  get rid of NOOP chains
     */
    for (inst = pp->firstinst; inst->type != TOK_END; inst++) {
        target = inst->l.next;
        while (target->type == TOK_NOP)
            target = target->l.next;
        inst->l.next = target;
    }

    /*
     *  The original allocation is for an area larger than
     *  necessary.  Reallocate to the actual space used
     *  and then relocate the code.
     */
    if ((par->freep - pp->firstinst)*2 > par->instcap)
        return pp;

    intptr_t ipp = (intptr_t)pp; // convert pointer to intptr_t!
    intptr_t new_allocsize = c_sizeof(_Reprog) + (par->freep - pp->firstinst)*c_sizeof(_Reinst);
    _Reprog *npp = (_Reprog *)i_realloc(pp, pp->allocsize, new_allocsize);
    ptrdiff_t diff = (intptr_t)npp - ipp;

    if ((npp == NULL) | (diff == 0))
        return (_Reprog *)ipp;
    npp->allocsize = new_allocsize;
    par->freep = (_Reinst *)((char *)par->freep + diff);

    for (inst = npp->firstinst; inst < par->freep; inst++) {
        switch (inst->type) {
        case TOK_OR:
        case TOK_STAR:
        case TOK_PLUS:
        case TOK_QUEST:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            break;
        case TOK_CCLASS:
        case TOK_NCCLASS:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            cl = inst->r.classp;
            cl->end = (_Rune *)((char*)cl->end + diff);
            break;
        }
        if (inst->l.left)
            inst->l.left = (_Reinst *)((char*)inst->l.left + diff);
    }
    npp->startinst = (_Reinst *)((char*)npp->startinst + diff);
    return npp;
}


static _Reclass*
_newclass(_Parser *par)
{
    if (par->nclass >= _NCLASS)
        _rcerror(par, CREG_TOOMANYCHARACTERCLASSES);
    return &(par->classp[par->nclass++]);
}


static int /* quoted */
_nextc(_Parser *par, _Rune *rp)
{
    int ret;
    for (;;) {
        ret = par->litmode;
        par->exprp += chartorune(rp, par->exprp);

        if (*rp == '\\') {
            if (par->litmode) {
                if (*par->exprp != 'E')
                    break;
                par->exprp += 1;
                par->litmode = false;
                continue;
            }
            par->exprp += chartorune(rp, par->exprp);
            if (*rp == 'Q') {
                par->litmode = true;
                continue;
            }
            ret = 1;
        }
        break;
    }
    return ret;
}


static void
_lexasciiclass(_Parser *par, _Rune *rp) /* assume *rp == '[' and *par->exprp == ':' */
{
    static struct { const char* c; int n, r; } cls[] = {
        {"alnum:]", 7, ASC_an}, {"alpha:]", 7, ASC_al}, {"ascii:]", 7, ASC_as},
        {"blank:]", 7, ASC_bl}, {"cntrl:]", 7, ASC_ct}, {"digit:]", 7, ASC_d},
        {"graph:]", 7, ASC_gr}, {"lower:]", 7, ASC_lo}, {"print:]", 7, ASC_pr},
        {"punct:]", 7, ASC_pu}, {"space:]", 7, ASC_s}, {"upper:]", 7, ASC_up},
        {"xdigit:]", 8, ASC_xd}, {"word:]", 6, ASC_w},
    };
    int inv = par->exprp[1] == '^', off = 1 + inv;
    for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i)
        if (!strncmp(par->exprp + off, cls[i].c, (size_t)cls[i].n)) {
            *rp = (_Rune)cls[i].r;
            par->exprp += off + cls[i].n;
            break;
        }
    if (par->rune_type == TOK_IRUNE && (*rp == ASC_lo || *rp == ASC_up))
        *rp = (_Rune)ASC_al;
    if (inv && *rp != '[')
        *rp += 1;
}


static void
_lexutfclass(_Parser *par, _Rune *rp)
{
    static struct { const char* c; uint32_t n, r; } cls[] = {
        {"{Alpha}", 7, UTF_al}, {"{L&}", 4, UTF_lc},
        {"{Digit}", 7, UTF_nd}, {"{Nd}", 4, UTF_nd},
        {"{Lower}", 7, UTF_ll}, {"{Ll}", 4, UTF_ll},
        {"{Upper}", 7, UTF_lu}, {"{Lu}", 4, UTF_lu},
        {"{Cntrl}", 7, UTF_cc}, {"{Cc}", 4, UTF_cc},
        {"{Alnum}", 7, UTF_an}, {"{Blank}", 7, UTF_bl},
        {"{Space}", 7, UTF_sp}, {"{Word}", 6, UTF_wr},
        {"{XDigit}", 8, ASC_xd},
        {"{Lt}", 4, UTF_lt}, {"{Nl}", 4, UTF_nl},
        {"{Pc}", 4, UTF_pc}, {"{Pd}", 4, UTF_pd},
        {"{Pf}", 4, UTF_pf}, {"{Pi}", 4, UTF_pi},
        {"{Zl}", 4, UTF_zl}, {"{Zp}", 4, UTF_zp},
        {"{Zs}", 4, UTF_zs}, {"{Sc}", 4, UTF_sc},
        {"{Arabic}", 8, UTF_arabic}, {"{Cyrillic}", 10, UTF_cyrillic},
        {"{Devanagari}", 10, UTF_devanagari}, {"{Greek}", 7, UTF_greek},
        {"{Han}", 5, UTF_han}, {"{Latin}", 7, UTF_latin},
    };
    unsigned inv = (*rp == 'P');
    for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i) {
        if (!strncmp(par->exprp, cls[i].c, (size_t)cls[i].n)) {
            if (par->rune_type == TOK_IRUNE && (cls[i].r == UTF_ll || cls[i].r == UTF_lu))
                *rp = (_Rune)(UTF_lc + inv);
            else
                *rp = (_Rune)(cls[i].r + inv);
            par->exprp += cls[i].n;
            break;
        }
    }
}

#define CASE_RUNE_MAPPINGS(rune) \
    case 't': rune = '\t'; break; \
    case 'n': rune = '\n'; break; \
    case 'r': rune = '\r'; break; \
    case 'v': rune = '\v'; break; \
    case 'f': rune = '\f'; break; \
    case 'a': rune = '\a'; break; \
    case 'd': rune = UTF_nd; break; \
    case 'D': rune = UTF_ND; break; \
    case 's': rune = UTF_sp; break; \
    case 'S': rune = UTF_SP; break; \
    case 'w': rune = UTF_wr; break; \
    case 'W': rune = UTF_WR; break


static _Token
_lex(_Parser *par)
{
    bool quoted = _nextc(par, &par->yyrune);

    if (quoted) {
        if (par->litmode)
            return par->rune_type;

        switch (par->yyrune) {
        CASE_RUNE_MAPPINGS(par->yyrune);
        case 'b': return TOK_WBOUND;
        case 'B': return TOK_NWBOUND;
        case 'A': return TOK_BOS;
        case 'z': return TOK_EOS;
        case 'Z': return TOK_EOZ;
        case 'x': /* hex number rune */
            if (*par->exprp != '{') break;
            sscanf(++par->exprp, "%x", &par->yyrune);
            while (*par->exprp) if (*(par->exprp++) == '}') break;
            if (par->exprp[-1] != '}')
                _rcerror(par, CREG_UNMATCHEDRIGHTPARENTHESIS);
            if (par->yyrune == 0) return TOK_END;
            break;
        case 'p': case 'P':
            _lexutfclass(par, &par->yyrune);
            break;
        }
        return par->rune_type;
    }

    switch (par->yyrune) {
    case  0 : return TOK_END;
    case '*': return TOK_STAR;
    case '?': return TOK_QUEST;
    case '+': return TOK_PLUS;
    case '|': return TOK_OR;
    case '^': return TOK_BOL;
    case '$': return TOK_EOL;
    case '.': return par->dot_type;
    case '[': return _bldcclass(par);
    case '(':
        if (par->exprp[0] == '?') { /* override global flags */
            for (int k = 1, enable = 1; ; ++k) switch (par->exprp[k]) {
                case  0 : par->exprp += k; return TOK_END;
                case ')': par->exprp += k + 1;
                          return TOK_CASED + (par->rune_type == TOK_IRUNE);
                case '-': enable = 0; break;
                case 's': par->dot_type = TOK_ANY + enable; break;
                case 'i': par->rune_type = TOK_RUNE + enable; break;
                default: _rcerror(par, CREG_UNKNOWNOPERATOR); return 0;
            }
        }
        return TOK_LBRA;
    case ')': return TOK_RBRA;
    }
    return par->rune_type;
}


static _Token
_bldcclass(_Parser *par)
{
    _Token type;
    _Rune r[_NCCRUNE];
    _Rune *p, *ep, *np;
    _Rune rune;
    int quoted;

    /* we have already seen the '[' */
    type = TOK_CCLASS;
    par->yyclassp = _newclass(par);

    /* look ahead for negation */
    /* SPECIAL CASE!!! negated classes don't match \n */
    ep = r;
    quoted = _nextc(par, &rune);
    if (!quoted && rune == '^') {
        type = TOK_NCCLASS;
        quoted = _nextc(par, &rune);
        ep[0] = ep[1] = '\n';
        ep += 2;
    }

    /* parse class into a set of spans */
    for (; ep < &r[_NCCRUNE]; quoted = _nextc(par, &rune)) {
        if (rune == 0) {
            _rcerror(par, CREG_MALFORMEDCHARACTERCLASS);
            return 0;
        }
        if (!quoted) {
            if (rune == ']')
                break;
            if (rune == '-') {
                if (ep != r && *par->exprp != ']') {
                    quoted = _nextc(par, &rune);
                    if (rune == 0) {
                        _rcerror(par, CREG_MALFORMEDCHARACTERCLASS);
                        return 0;
                    }
                    ep[-1] = par->rune_type == TOK_IRUNE ? utf8_casefold(rune) : rune;
                    continue;
                }
            }
            if (rune == '[' && *par->exprp == ':')
                _lexasciiclass(par, &rune);
        } else switch (rune) {
            CASE_RUNE_MAPPINGS(rune);
            case 'p': case 'P':
                _lexutfclass(par, &rune);
                break;
        }
        ep[0] = ep[1] = par->rune_type == TOK_IRUNE ? utf8_casefold(rune) : rune;
        ep += 2;
    }

    /* sort on span start */
    for (p = r; p < ep; p += 2)
        for (np = p; np < ep; np += 2)
            if (*np < *p) {
                rune = np[0]; np[0] = p[0]; p[0] = rune;
                rune = np[1]; np[1] = p[1]; p[1] = rune;
            }

    /* merge spans */
    np = par->yyclassp->spans;
    p = r;
    if (r == ep)
        par->yyclassp->end = np;
    else {
        np[0] = *p++;
        np[1] = *p++;
        for (; p < ep; p += 2)
            if (p[0] <= np[1]) {
                if (p[1] > np[1])
                    np[1] = p[1];
            } else {
                np += 2;
                np[0] = p[0];
                np[1] = p[1];
            }
        par->yyclassp->end = np+2;
    }

    return type;
}


static _Reprog*
_regcomp1(_Reprog *pp, _Parser *par, const char *s, int cflags)
{
    _Token token;

    /* get memory for the program. estimated max usage */
    intptr_t instcap = 5 + 6*c_strlen(s);
    intptr_t new_allocsize = c_sizeof(_Reprog) + instcap*c_sizeof(_Reinst);
    pp = (_Reprog *)i_realloc(pp, pp ? pp->allocsize : 0, new_allocsize);
    if (! pp) {
        par->error = CREG_OUTOFMEMORY;
        return NULL;
    }
    pp->allocsize = new_allocsize;
    pp->flags.icase = (cflags & CREG_ICASE) != 0;
    pp->flags.dotall = (cflags & CREG_DOTALL) != 0;
    par->instcap = instcap;
    par->freep = pp->firstinst;
    par->classp = pp->cclass;
    par->error = 0;

    if (setjmp(par->regkaboom))
        goto out;

    /* go compile the sucker */
    par->flags = pp->flags;
    par->rune_type = pp->flags.icase ? TOK_IRUNE : TOK_RUNE;
    par->dot_type = pp->flags.dotall ? TOK_ANYNL : TOK_ANY;
    par->litmode = false;
    par->exprp = s;
    par->nclass = 0;
    par->nbra = 0;
    par->atorp = par->atorstack;
    par->andp = par->andstack;
    par->subidp = par->subidstack;
    par->lastwasand = false;
    par->cursubid = 0;

    /* Start with a low priority operator to prime parser */
    _pushator(par, TOK_START-1);
    while ((token = _lex(par)) != TOK_END) {
        if ((token & TOK_MASK) == TOK_OPERATOR)
            _operator(par, token);
        else
            _operand(par, token);
    }

    /* Close with a low priority operator */
    _evaluntil(par, TOK_START);

    /* Force TOK_END */
    _operand(par, TOK_END);
    _evaluntil(par, TOK_START);

    if (par->nbra)
        _rcerror(par, CREG_UNMATCHEDLEFTPARENTHESIS);
    --par->andp;    /* points to first and only _operand */
    pp->startinst = par->andp->first;

    pp = _optimize(par, pp);
    pp->nsubids = par->cursubid;
out:
    if (par->error) {
        i_free(pp, pp->allocsize);
        pp = NULL;
    }
    return pp;
}

#if defined __clang__
  #pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#elif defined __GNUC__
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

static int
_runematch(_Rune s, _Rune r)
{
    int inv = 0, n;
    switch (s) {
    case ASC_D: inv = 1; case ASC_d: return inv ^ (isdigit((int)r) != 0);
    case ASC_S: inv = 1; case ASC_s: return inv ^ (isspace((int)r) != 0);
    case ASC_W: inv = 1; case ASC_w: return inv ^ ((isalnum((int)r) != 0) | (r == '_'));
    case ASC_AL: inv = 1; case ASC_al: return inv ^ (isalpha((int)r) != 0);
    case ASC_AN: inv = 1; case ASC_an: return inv ^ (isalnum((int)r) != 0);
    case ASC_AS: return (r >= 128); case ASC_as: return (r < 128);
    case ASC_BL: inv = 1; case ASC_bl: return inv ^ ((r == ' ') | (r == '\t'));
    case ASC_CT: inv = 1; case ASC_ct: return inv ^ (iscntrl((int)r) != 0);
    case ASC_GR: inv = 1; case ASC_gr: return inv ^ (isgraph((int)r) != 0);
    case ASC_PR: inv = 1; case ASC_pr: return inv ^ (isprint((int)r) != 0);
    case ASC_PU: inv = 1; case ASC_pu: return inv ^ (ispunct((int)r) != 0);
    case ASC_LO: inv = 1; case ASC_lo: return inv ^ (islower((int)r) != 0);
    case ASC_UP: inv = 1; case ASC_up: return inv ^ (isupper((int)r) != 0);
    case ASC_XD: inv = 1; case ASC_xd: return inv ^ (isxdigit((int)r) != 0);
    case UTF_AN: inv = 1; case UTF_an: return inv ^ (int)utf8_isalnum(r);
    case UTF_BL: inv = 1; case UTF_bl: return inv ^ (int)utf8_isblank(r);
    case UTF_SP: inv = 1; case UTF_sp: return inv ^ (int)utf8_isspace(r);
    case UTF_LL: inv = 1; case UTF_ll: return inv ^ (int)utf8_islower(r);
    case UTF_LU: inv = 1; case UTF_lu: return inv ^ (int)utf8_isupper(r);
    case UTF_LC: inv = 1; case UTF_lc: return inv ^ (int)utf8_iscased(r);
    case UTF_AL: inv = 1; case UTF_al: return inv ^ (int)utf8_isalpha(r);
    case UTF_WR: inv = 1; case UTF_wr: return inv ^ (int)utf8_isword(r);
    case UTF_cc: case UTF_CC:
    case UTF_lt: case UTF_LT:
    case UTF_nd: case UTF_ND:
    case UTF_nl: case UTF_NL:
    case UTF_pc: case UTF_PC:
    case UTF_pd: case UTF_PD:
    case UTF_pf: case UTF_PF:
    case UTF_pi: case UTF_PI:
    case UTF_sc: case UTF_SC:
    case UTF_zl: case UTF_ZL:
    case UTF_zp: case UTF_ZP:
    case UTF_zs: case UTF_ZS:
    case UTF_arabic: case UTF_ARABIC:
    case UTF_cyrillic: case UTF_CYRILLIC:
    case UTF_devanagari: case UTF_DEVANAGARI:
    case UTF_greek: case UTF_GREEK:
    case UTF_han: case UTF_HAN:
    case UTF_latin: case UTF_LATIN:
        n = (int)s - UTF_GRP;
        inv = n & 1;
        return inv ^ (int)utf8_isgroup(n / 2, r);
    }
    return s == r;
}

/*
 *  return 0 if no match
 *        >0 if a match
 *        <0 if we ran out of _relist space
 */
static int
_regexec1(const _Reprog *progp,  /* program to run */
    const char *bol,    /* string to run machine on */
    _Resub *mp,         /* subexpression elements */
    int ms,        /* number of elements at mp */
    _Reljunk *j,
    int mflags
)
{
    int flag=0;
    _Reinst *inst;
    _Relist *tlp;
    _Relist *tl, *nl;    /* This list, next list */
    _Relist *tle, *nle;  /* Ends of this and next list */
    const char *s, *p;
    _Rune r, *rp, *ep;
    int n, checkstart, match = 0;
    int i;

    bool icase = progp->flags.icase;
    checkstart = j->starttype;
    if (mp)
        for (i=0; i<ms; i++) {
            mp[i].buf = NULL;
            mp[i].size = 0;
        }
    j->relist[0][0].inst = NULL;
    j->relist[1][0].inst = NULL;

    /* Execute machine once for each character, including terminal NUL */
    s = j->starts;
    do {
        /* fast check for first char */
        if (checkstart) {
            switch (j->starttype) {
            case TOK_IRUNE:
                p = utfruneicase(s, j->startchar);
                goto next1;
            case TOK_RUNE:
                p = utfrune(s, j->startchar);
                next1:
                if (p == NULL || s == j->eol)
                    return match;
                s = p;
                break;
            case TOK_BOL:
                if (s == bol)
                    break;
                p = utfrune(s, '\n');
                if (p == NULL || s == j->eol)
                    return match;
                s = p+1;
                break;
            }
        }
        r = *(unsigned char*)s;
        n = r < 128 ? 1 : chartorune(&r, s);

        /* switch run lists */
        tl = j->relist[flag];
        tle = j->reliste[flag];
        nl = j->relist[flag^=1];
        nle = j->reliste[flag];
        nl->inst = NULL;

        /* Add first instruction to current list */
        if (match == 0)
            _renewemptythread(tl, progp->startinst, ms, s);

        /* Execute machine until current list is empty */
        for (tlp=tl; tlp->inst; tlp++) {    /* assignment = */
            for (inst = tlp->inst; ; inst = inst->l.next) {
                int ok = false;

                switch (inst->type) {
                case TOK_IRUNE:
                    r = utf8_casefold(r); /* fall through */
                case TOK_RUNE:
                    ok = _runematch(inst->r.rune, r);
                    break;
                case TOK_CASED: case TOK_ICASE:
                    icase = inst->type == TOK_ICASE;
                    continue;
                case TOK_LBRA:
                    tlp->se.m[inst->r.subid].buf = s;
                    continue;
                case TOK_RBRA:
                    tlp->se.m[inst->r.subid].size = (s - tlp->se.m[inst->r.subid].buf);
                    continue;
                case TOK_ANY:
                    ok = (r != '\n');
                    break;
                case TOK_ANYNL:
                    ok = true;
                    break;
                case TOK_BOL:
                    if (s == bol || s[-1] == '\n') continue;
                    break;
                case TOK_BOS:
                    if (s == bol) continue;
                    break;
                case TOK_EOL:
                    if (r == '\n') continue; /* fall through */
                case TOK_EOS:
                    if (s == j->eol || r == 0) continue;
                    break;
                case TOK_EOZ:
                    if (s == j->eol || r == 0 || (r == '\n' && s[1] == 0)) continue;
                    break;
                case TOK_NWBOUND:
                    ok = true; /* fall through */
                case TOK_WBOUND:
                    if (ok ^ (s == bol || s == j->eol || (utf8_isword(utf8_peek_off(s, -1))
                                                        ^ utf8_isword(utf8_peek(s)))))
                        continue;
                    break;
                case TOK_NCCLASS:
                    ok = true; /* fall through */
                case TOK_CCLASS:
                    ep = inst->r.classp->end;
                    if (icase) r = utf8_casefold(r);
                    for (rp = inst->r.classp->spans; rp < ep; rp += 2) {
                        if ((r >= rp[0] && r <= rp[1]) || (rp[0] == rp[1] && _runematch(rp[0], r)))
                            break;
                    }
                    ok ^= (rp < ep);
                    break;
                case TOK_OR:
                    /* evaluate right choice later */
                    if (_renewthread(tlp, inst->r.right, ms, &tlp->se) == tle)
                        return -1;
                    /* efficiency: advance and re-evaluate */
                    continue;
                case TOK_END:    /* Match! */
                    match = !(mflags & CREG_FULLMATCH) ||
                            ((s == j->eol || r == 0 || r == '\n') &&
                            (tlp->se.m[0].buf == bol || tlp->se.m[0].buf[-1] == '\n'));
                    tlp->se.m[0].size = (s - tlp->se.m[0].buf);
                    if (mp != NULL)
                        _renewmatch(mp, ms, &tlp->se, progp->nsubids);
                    break;
                }

                if (ok && _renewthread(nl, inst->l.next, ms, &tlp->se) == nle)
                    return -1;
                break;
            }
        }
        if (s == j->eol)
            break;
        checkstart = j->starttype && nl->inst==NULL;
        s += n;
    } while (r);
    return match;
}


static int
_regexec2(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    _Resub *mp,         /* subexpression elements */
    int ms,             /* number of elements at mp */
    _Reljunk *j,
    int mflags
)
{
    int rv;
    _Relist *relists;

    /* mark space */
    intptr_t sz = 2 * _BIGLISTSIZE*c_sizeof(_Relist);
    relists = (_Relist *)i_malloc(sz);
    if (relists == NULL)
        return -1;

    j->relist[0] = relists;
    j->relist[1] = relists + _BIGLISTSIZE;
    j->reliste[0] = relists + _BIGLISTSIZE - 2;
    j->reliste[1] = relists + 2*_BIGLISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, j, mflags);
    i_free(relists, sz);
    return rv;
}

static int
_regexec(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    int ms,             /* number of elements at mp */
    _Resub mp[],        /* subexpression elements */
    int mflags)
{
    _Reljunk j;
    _Relist relist0[_LISTSIZE], relist1[_LISTSIZE];
    int rv;

    /*
     *  use user-specified starting/ending location if specified
     */
    j.starts = bol;
    j.eol = NULL;

    if (mp && mp[0].size) {
        if (mflags & CREG_STARTEND)
            j.starts = mp[0].buf, j.eol = mp[0].buf + mp[0].size;
        else if (mflags & CREG_NEXT)
            j.starts = mp[0].buf + mp[0].size;
    }

    j.starttype = 0;
    j.startchar = 0;
    int rune_type = progp->flags.icase ? TOK_IRUNE : TOK_RUNE;
    if (progp->startinst->type == rune_type && progp->startinst->r.rune < 128) {
        j.starttype = rune_type;
        j.startchar = progp->startinst->r.rune;
    }
    if (progp->startinst->type == TOK_BOL)
        j.starttype = TOK_BOL;

    /* mark space */
    j.relist[0] = relist0;
    j.relist[1] = relist1;
    j.reliste[0] = relist0 + _LISTSIZE - 2;
    j.reliste[1] = relist1 + _LISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, &j, mflags);
    if (rv >= 0)
        return rv;
    rv = _regexec2(progp, bol, mp, ms, &j, mflags);
    return rv;
}


static void
_build_subst(const char* replace, int nmatch, const csview match[],
             bool (*mfun)(int, csview, cstr*), cstr* subst) {
    cstr_buf buf = cstr_buffer(subst);
    intptr_t len = 0, cap = buf.cap;
    char* dst = buf.data;
    cstr mstr = {0};

    while (*replace != '\0') {
        if (*replace == '$') {
            const int arg = *++replace;
            int g;
            switch (arg) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                g = arg - '0';
                if (replace[1] >= '0' && replace[1] <= '9' && replace[2] == ';')
                    { g = g*10 + (replace[1] - '0'); replace += 2; }
                if (g < nmatch) {
                    csview m = mfun && mfun(g, match[g], &mstr) ? cstr_sv(&mstr) : match[g];
                    if (len + m.size > cap)
                        dst = cstr_reserve(subst, cap += cap/2 + m.size);
                    for (int i = 0; i < m.size; ++i)
                        dst[len++] = m.buf[i];
                }
                ++replace;
            case '\0':
                continue;
            }
        }
        if (len == cap)
            dst = cstr_reserve(subst, cap += cap/2 + 4);
        dst[len++] = *replace++;
    }
    cstr_drop(&mstr);
    _cstr_set_size(subst, len);
}


/* ---------------------------------------------------------------
 * API functions
 */

int
cregex_compile_3(cregex *self, const char* pattern, int cflags) {
    _Parser par;
    self->prog = _regcomp1(self->prog, &par, pattern, cflags);
    return self->error = par.error;
}

int
cregex_captures(const cregex* self) {
    return self->prog ? self->prog->nsubids : 0;
}

int
cregex_find_4(const cregex* re, const char* input, csview match[], int mflags) {
    int res = _regexec(re->prog, input, cregex_captures(re) + 1, match, mflags);
    switch (res) {
    case 1: return CREG_OK;
    case 0: return CREG_NOMATCH;
    default: return CREG_MATCHERROR;
    }
}

int
cregex_find_pattern_4(const char* pattern, const char* input,
                      csview match[], int cmflags) {
    cregex re = {0};
    int res = cregex_compile(&re, pattern, cmflags);
    if (res != CREG_OK) return res;
    res = cregex_find(&re, input, match, cmflags);
    cregex_drop(&re);
    return res;
}

cstr
cregex_replace_sv_6(const cregex* re, csview input, const char* replace, int count,
                    bool (*mfun)(int, csview, cstr*), int rflags) {
    cstr out = {0};
    cstr subst = {0};
    csview match[CREG_MAX_CAPTURES];
    int nmatch = cregex_captures(re) + 1;
    if (!count) count = INT32_MAX;
    bool copy = !(rflags & CREG_STRIP);

    while (count-- && cregex_find_sv(re, input, match) == CREG_OK) {
        _build_subst(replace, nmatch, match, mfun, &subst);
        const intptr_t mpos = (match[0].buf - input.buf);
        if (copy & (mpos > 0)) cstr_append_n(&out, input.buf, mpos);
        cstr_append_s(&out, subst);
        input.buf = match[0].buf + match[0].size;
        input.size -= mpos + match[0].size;
    }
    if (copy) cstr_append_sv(&out, input);
    cstr_drop(&subst);
    return out;
}

cstr
cregex_replace_pattern_6(const char* pattern, const char* input, const char* replace, int count,
                         bool (*mfun)(int, csview, cstr*), int crflags) {
    cregex re = {0};
    if (cregex_compile(&re, pattern, crflags) != CREG_OK)
        assert(0);
    csview sv = c_sv(input, c_strlen(input));
    cstr out = cregex_replace_sv(&re, sv, replace, count, mfun, crflags);
    cregex_drop(&re);
    return out;
}

void
cregex_drop(cregex* self) {
    i_free(self->prog, self->prog->allocsize);
}

#endif // STC_CREGEX_PRV_C_INCLUDED
