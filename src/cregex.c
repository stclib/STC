/*
This is a Unix port of the Plan 9 regular expression library, by Rob Pike.
Please send comments about the packaging to Russ Cox <rsc@swtch.com>.

Copyright © 2021 Plan 9 Foundation
Copyright © 2022 Tyge Løvset, for additions made in 2022.

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

#include <stc/cregex.h>
#include <stc/utf8.h>
#define i_header
#include <stc/cstr.h>
#include <setjmp.h>
#include <ctype.h>

typedef uint32_t _Rune; /* Utf8 code point */
typedef int32_t _Token;
/* max character classes per program */
#define _NCLASS cre_MAXCLASSES
/* max subexpressions */
#define _NSUBEXP cre_MAXCAPTURES
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
        int     subid;          /* sub-expression id for RE_RBRA and RE_LBRA */
        struct _Reinst *right;   /* right child of RE_OR */
    } r;
    union {    /* regexp relies on these two being in the same union */
        struct _Reinst *left;    /* left child of RE_OR */
        struct _Reinst *next;    /* next instruction for RE_CAT & RE_LBRA */
    } l;
} _Reinst;

typedef struct {
    bool caseless;
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
 *  0x810000-0x81FFFF: RE_RUNE and char classes.
 *  0x820000-0x82FFFF: tokens, i.e. operands for operators
 */
enum {
    RE_MASK        = 0xFF00000,
    RE_OPERATOR    = 0x8000000, /* Bitmask of all operators */
    RE_START       = 0x8000001, /* Start, used for marker on stack */
    RE_RBRA        ,           /* Right bracket, ) */
    RE_LBRA        ,           /* Left bracket, ( */
    RE_OR          ,           /* Alternation, | */
    RE_CAT         ,           /* Concatentation, implicit operator */
    RE_STAR        ,           /* Closure, * */
    RE_PLUS        ,           /* a+ == aa* */
    RE_QUEST       ,           /* a? == a|nothing, i.e. 0 or 1 a's */
    RE_RUNE        = 0x8100000,
    RE_IRUNE,
    ASC_an      , ASC_AN,   /* alphanum */
    ASC_al      , ASC_AL,   /* alpha */
    ASC_as      , ASC_AS,   /* ascii */
    ASC_bl      , ASC_BL,   /* blank */
    ASC_ct      , ASC_CT,   /* ctrl */
    ASC_d       , ASC_D,    /* digit */
    ASC_s       , ASC_S,    /* space */
    ASC_w       , ASC_W,    /* word */
    ASC_gr      , ASC_GR,   /* graphic */
    ASC_pr      , ASC_PR,   /* print */
    ASC_pu      , ASC_PU,   /* punct */
    ASC_lo      , ASC_LO,   /* lower */
    ASC_up      , ASC_UP,   /* upper */
    ASC_xd      , ASC_XD,   /* hex */
    UTF_d       , UTF_D,    /* utf dec digit, non-digit */
    UTF_s       , UTF_S,    /* utf8 white space */
    UTF_w       , UTF_W,    /* utf8 word */
    UTF_al      , UTF_AL,   /* utf8 letter cased */
    UTF_lo      , UTF_LO,   /* utf8 letter lower */
    UTF_up      , UTF_UP,   /* utf8 letter upper */
    UTF_xd      , UTF_XD,   /* utf8 hex digit */
    UTF_an      , UTF_AN,   /* utf8 alphanumeric */
    RE_ANY         = 0x8200000, /* Any character except newline, . */
    RE_ANYNL       ,           /* Any character including newline, . */
    RE_NOP         ,           /* No operation, internal use only */
    RE_BOL         , RE_BOS,      /* Beginning of line, string, ^ */
    RE_EOL         , RE_EOS, RE_EOZ, /* End of line, string, $ */
    RE_CCLASS      ,           /* Character class, [] */
    RE_NCCLASS     ,           /* Negated character class, [] */
    RE_WBOUND      ,           /* Non-word boundary, not consuming meta char */
    RE_NWBOUND     ,           /* Word boundary, not consuming meta char */
    RE_END         = 0x82FFFFF, /* Terminate: match found */
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
    return (const char*)b - s;
}

static const char*
utfrune(const char *s, _Rune c)
{
    _Rune r;

    if (c < 128)        /* ascii */
        return strchr((char *)s, c);

    for (;;) {
        int n = chartorune(&r, s);
        if (r == c) return s;
        if ((r == 0) | (n == 0)) return NULL;
        s += n;
    }
}

static const char*
utfruneicase(const char *s, _Rune c)
{
    _Rune r;
    c = utf8_casefold(c);
    for (;;) {
        int n = chartorune(&r, s);
        if (utf8_casefold(r) == c) return s;
        if ((r == 0) | (n == 0)) return NULL;
        s += n;
    }
}

/************
 * regaux.c *
 ************/

/*
 *  save a new match in mp
 */
static void
_renewmatch(_Resub *mp, unsigned ms, _Resublist *sp, int nsubids)
{
    int i;

    if (mp==NULL || ms==0)
        return;
    if (mp[0].str == NULL || sp->m[0].str < mp[0].str ||
       (sp->m[0].str == mp[0].str && sp->m[0].size > mp[0].size)) {
        for (i=0; i<ms && i<=nsubids; i++)
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
    unsigned ms,
    _Resublist *sep)       /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sep->m[0].str < p->se.m[0].str) {
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
    unsigned ms,
    const char *sp)             /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sp < p->se.m[0].str) {
                if (ms > 1)
                    memset(&p->se, 0, sizeof(p->se));
                p->se.m[0].str = sp;
            }
            return 0;
        }
    }
    p->inst = ip;
    if (ms > 1)
        memset(&p->se, 0, sizeof(p->se));
    p->se.m[0].str = sp;
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
    bool lexdone;
    short nbra;
    short nclass;
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
        _operator(par, RE_CAT);    /* catenate is implicit */
    i = _newinst(par, t);

    if ((t == RE_CCLASS) | (t == RE_NCCLASS))
        i->r.classp = par->yyclassp;
    if ((t == RE_RUNE) | (t == RE_IRUNE))
        i->r.rune = par->yyrune;

    _pushand(par, i, i);
    par->lastwasand = true;
}

static void
_operator(_Parser *par, _Token t)
{
    if (t==RE_RBRA && --par->nbra<0)
        _rcerror(par, cre_unmatchedrightparenthesis);
    if (t==RE_LBRA) {
        if (++par->cursubid >= _NSUBEXP)
            _rcerror(par, cre_toomanysubexpressions);
        par->nbra++;
        if (par->lastwasand)
            _operator(par, RE_CAT);
    } else
        _evaluntil(par, t);
    if (t != RE_RBRA)
        _pushator(par, t);
    par->lastwasand = 0;
    if (t==RE_STAR || t==RE_QUEST || t==RE_PLUS || t==RE_RBRA)
        par->lastwasand = true;    /* these look like operands */
}

static void
_pushand(_Parser *par, _Reinst *f, _Reinst *l)
{
    if (par->andp >= &par->andstack[_NSTACK])
        _rcerror(par, cre_operandstackoverflow);
    par->andp->first = f;
    par->andp->last = l;
    par->andp++;
}

static void
_pushator(_Parser *par, _Token t)
{
    if (par->atorp >= &par->atorstack[_NSTACK])
        _rcerror(par, cre_operatorstackoverflow);
    *par->atorp++ = t;
    *par->subidp++ = par->cursubid;
}

static _Node*
_popand(_Parser *par, _Token op)
{
    _Reinst *inst;

    if (par->andp <= &par->andstack[0]) {
        _rcerror(par, cre_missingoperand);
        inst = _newinst(par, RE_NOP);
        _pushand(par, inst, inst);
    }
    return --par->andp;
}

static _Token
_popator(_Parser *par)
{
    if (par->atorp <= &par->atorstack[0])
        _rcerror(par, cre_operatorstackunderflow);
    --par->subidp;
    return *--par->atorp;
}

static void
_evaluntil(_Parser *par, _Token pri)
{
    _Node *op1, *op2;
    _Reinst *inst1, *inst2;

    while (pri==RE_RBRA || par->atorp[-1]>=pri) {
        switch (_popator(par)) {
        default:
            _rcerror(par, cre_unknownoperator);
            break;
        case RE_LBRA:        /* must have been RE_RBRA */
            op1 = _popand(par, '(');
            inst2 = _newinst(par, RE_RBRA);
            inst2->r.subid = *par->subidp;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, RE_LBRA);
            inst1->r.subid = *par->subidp;
            inst1->l.next = op1->first;
            _pushand(par, inst1, inst2);
            return;
        case RE_OR:
            op2 = _popand(par, '|');
            op1 = _popand(par, '|');
            inst2 = _newinst(par, RE_NOP);
            op2->last->l.next = inst2;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, RE_OR);
            inst1->r.right = op1->first;
            inst1->l.left = op2->first;
            _pushand(par, inst1, inst2);
            break;
        case RE_CAT:
            op2 = _popand(par, 0);
            op1 = _popand(par, 0);
            op1->last->l.next = op2->first;
            _pushand(par, op1->first, op2->last);
            break;
        case RE_STAR:
            op2 = _popand(par, '*');
            inst1 = _newinst(par, RE_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, inst1, inst1);
            break;
        case RE_PLUS:
            op2 = _popand(par, '+');
            inst1 = _newinst(par, RE_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, op2->first, inst1);
            break;
        case RE_QUEST:
            op2 = _popand(par, '?');
            inst1 = _newinst(par, RE_OR);
            inst2 = _newinst(par, RE_NOP);
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
    for (inst = pp->firstinst; inst->type != RE_END; inst++) {
        target = inst->l.next;
        while (target->type == RE_NOP)
            target = target->l.next;
        inst->l.next = target;
    }

    /*
     *  The original allocation is for an area larger than
     *  necessary.  Reallocate to the actual space used
     *  and then relocate the code.
     */
    uintptr_t ipp = (uintptr_t)pp;
    size_t size = sizeof(_Reprog) + (par->freep - pp->firstinst)*sizeof(_Reinst);
    _Reprog *npp = (_Reprog *)c_realloc(pp, size);
    ptrdiff_t diff = (uintptr_t)npp - ipp;

    if ((npp == NULL) | (diff == 0))
        return (_Reprog *)ipp;
    par->freep = (_Reinst *)((char *)par->freep + diff);

    for (inst = npp->firstinst; inst < par->freep; inst++) {
        switch (inst->type) {
        case RE_OR:
        case RE_STAR:
        case RE_PLUS:
        case RE_QUEST:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            break;
        case RE_CCLASS:
        case RE_NCCLASS:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            cl = inst->r.classp;
            cl->end = (_Rune *)((char*)cl->end + diff);
            break;
        }
        inst->l.left = (_Reinst *)((char*)inst->l.left + diff);
    }
    npp->startinst = (_Reinst *)((char*)npp->startinst + diff);
    return npp;
}

static _Reclass*
_newclass(_Parser *par)
{
    if (par->nclass >= _NCLASS)
        _rcerror(par, cre_toomanycharacterclasses);
    return &(par->classp[par->nclass++]);
}

static int
_nextc(_Parser *par, _Rune *rp)
{
    if (par->lexdone) {
        *rp = 0;
        return 1;
    }
    par->exprp += chartorune(rp, par->exprp);
    if (*rp == '\\') {
        if (par->litmode && *par->exprp != 'E')
            return 1;
        par->exprp += chartorune(rp, par->exprp);
        switch (*rp) {
            case 'E': return par->litmode + 1;
            case 't': *rp = '\t'; break;
            case 'n': *rp = '\n'; break;
            case 'r': *rp = '\r'; break;
            case 'v': *rp = '\v'; break;
            case 'f': *rp = '\f'; break;
            case 'd': *rp = UTF_d; break;
            case 'D': *rp = UTF_D; break;
            case 's': *rp = UTF_s; break;
            case 'S': *rp = UTF_S; break;
            case 'w': *rp = UTF_w; break;
            case 'W': *rp = UTF_W; break;
            case 'x': if (*par->exprp != '{') break;
                *rp = 0; sscanf(++par->exprp, "%x", rp);
                while (*par->exprp) if (*(par->exprp++) == '}') break;
                if (par->exprp[-1] != '}')
                    _rcerror(par, cre_unmatchedrightparenthesis);
                return 2;
            case 'p': case 'P': { /* https://www.regular-expressions.info/unicode.html */
                static struct { const char* c; int n, r; } cls[] = {
                    {"{Space}", 7, UTF_s}, {"{Zs}", 4, UTF_s},
                    {"{Digit}", 7, UTF_d}, {"{Nd}", 4, UTF_d},
                    {"{Alpha}", 7, UTF_al}, {"{LC}", 4, UTF_al},
                    {"{Lower}", 7, UTF_lo}, {"{Ll}", 4, UTF_lo},
                    {"{Upper}", 7, UTF_up}, {"{Lu}", 4, UTF_up},
                    {"{Alnum}", 7, UTF_an},
                    {"{XDigit}", 8, UTF_xd},
                };
                int inv = *rp == 'P';
                for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i)
                    if (!strncmp(par->exprp, cls[i].c, cls[i].n)) {
                        if (par->rune_type == RE_IRUNE && (cls[i].r == UTF_lo || cls[i].r == UTF_up))
                            *rp = UTF_al + inv;
                        else
                            *rp = cls[i].r + inv;
                        par->exprp += cls[i].n;
                        break;
                    }
                if (*rp < RE_OPERATOR) {
                    _rcerror(par, cre_unknownoperator);
                    *rp = 0;
                }
                break;
            }
        }
        return 1;
    }
    if (*rp == 0)
        par->lexdone = true;
    return par->litmode;
}

static _Token
_lex(_Parser *par)
{
    int quoted;
    start:
    quoted = _nextc(par, &par->yyrune);
    if (quoted) {
        if (quoted == 2) {
            if (par->litmode && par->yyrune == 'E') {
                par->litmode = false;
                goto start;
            }
            return par->yyrune == 0 ? RE_END : par->rune_type;
        }
        switch (par->yyrune) {
        case  0 : return RE_END;
        case 'b': return RE_WBOUND;
        case 'B': return RE_NWBOUND;
        case 'A': return RE_BOS;
        case 'z': return RE_EOS;
        case 'Z': return RE_EOZ;
        case 'Q': par->litmode = true;
                  goto start;
        default : return par->rune_type;
        }
    }

    switch (par->yyrune) {
    case  0 : return RE_END;
    case '*': return RE_STAR;
    case '?': return RE_QUEST;
    case '+': return RE_PLUS;
    case '|': return RE_OR;
    case '.': return par->dot_type;
    case '(':
        if (par->exprp[0] == '?') { /* override global flags */
            for (int k = 1, enable = 1; ; ++k) switch (par->exprp[k]) {
                case  0 : par->exprp += k; return RE_END;
                case ')': par->exprp += k + 1; goto start;
                case '-': enable = 0; break;
                case 's': par->dot_type = RE_ANY + enable; break;
                case 'i': par->rune_type = RE_RUNE + enable; break;
                default: _rcerror(par, cre_unknownoperator); return 0;
            }
        }
        return RE_LBRA;
    case ')': return RE_RBRA;
    case '^': return RE_BOL;
    case '$': return RE_EOL;
    case '[': return _bldcclass(par);
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
    type = RE_CCLASS;
    par->yyclassp = _newclass(par);

    /* look ahead for negation */
    /* SPECIAL CASE!!! negated classes don't match \n */
    ep = r;
    quoted = _nextc(par, &rune);
    if (!quoted && rune == '^') {
        type = RE_NCCLASS;
        quoted = _nextc(par, &rune);
        *ep++ = '\n';
        *ep++ = '\n';
    }

    /* parse class into a set of spans */
    for (; ep < &r[_NCCRUNE]; quoted = _nextc(par, &rune)) {
        if (rune == 0) {
            _rcerror(par, cre_malformedcharacterclass);
            return 0;
        }
        if (!quoted) {
            if (rune == ']')
                break;
            if (rune == '-') {
                if (ep != r && *par->exprp != ']') {
                    quoted = _nextc(par, &rune);
                    if (rune == 0) {
                        _rcerror(par, cre_malformedcharacterclass);
                        return 0;
                    }
                    ep[-1] = rune;
                    continue;
                }
            }
            if (rune == '[' && *par->exprp == ':') {
                static struct { const char* c; int n, r; } cls[] = {
                    {"alnum:]", 7, ASC_an}, {"alpha:]", 7, ASC_al}, {"ascii:]", 7, ASC_as},
                    {"blank:]", 7, ASC_bl}, {"cntrl:]", 7, ASC_ct}, {"digit:]", 7, ASC_d},
                    {"graph:]", 7, ASC_gr}, {"lower:]", 7, ASC_lo}, {"print:]", 7, ASC_pr},
                    {"punct:]", 7, ASC_pu}, {"space:]", 7, ASC_s}, {"upper:]", 7, ASC_up},
                    {"xdigit:]", 8, ASC_xd}, {"word:]", 6, ASC_w},
                };
                int inv = par->exprp[1] == '^', off = 1 + inv;
                for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i)
                    if (!strncmp(par->exprp + off, cls[i].c, cls[i].n)) {
                        rune = cls[i].r;
                        par->exprp += off + cls[i].n;
                        break;
                    }
                if (par->rune_type == RE_IRUNE && (rune == ASC_lo || rune == ASC_up))
                    rune = ASC_al;
                if (inv && rune != '[')
                    rune += 1;
            }
        }
        *ep++ = rune;
        *ep++ = rune;
    }

    /* sort on span start */
    for (p = r; p < ep; p += 2) {
        for (np = p; np < ep; np += 2)
            if (*np < *p) {
                rune = np[0];
                np[0] = p[0];
                p[0] = rune;
                rune = np[1];
                np[1] = p[1];
                p[1] = rune;
            }
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
_regcomp1(_Reprog *progp, _Parser *par, const char *s, int cflags)
{
    _Token token;

    /* get memory for the program. estimated max usage */
    const int instcap = 5 + 6*strlen(s);
    _Reprog* pp = (_Reprog *)c_realloc(progp, sizeof(_Reprog) + instcap*sizeof(_Reinst));
    if (pp == NULL) {
        par->error = cre_outofmemory;
        c_free(progp);
        return NULL;
    }
    pp->flags.caseless = (cflags & cre_c_caseless) != 0;
    pp->flags.dotall = (cflags & cre_c_dotall) != 0;
    par->freep = pp->firstinst;
    par->classp = pp->cclass;
    par->error = 0;

    if (setjmp(par->regkaboom))
        goto out;

    /* go compile the sucker */
    par->lexdone = false;
    par->flags = pp->flags;
    par->rune_type = pp->flags.caseless ? RE_IRUNE : RE_RUNE;
    par->dot_type = pp->flags.dotall ? RE_ANYNL : RE_ANY;
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
    _pushator(par, RE_START-1);
    while ((token = _lex(par)) != RE_END) {
        if ((token & RE_MASK) == RE_OPERATOR)
            _operator(par, token);
        else
            _operand(par, token);
    }

    /* Close with a low priority operator */
    _evaluntil(par, RE_START);

    /* Force RE_END */
    _operand(par, RE_END);
    _evaluntil(par, RE_START);
#ifdef DEBUG
    dumpstack(par);
#endif
    if (par->nbra)
        _rcerror(par, cre_unmatchedleftparenthesis);
    --par->andp;    /* points to first and only _operand */
    pp->startinst = par->andp->first;
#ifdef DEBUG
    dump(pp);
#endif
    pp = _optimize(par, pp);
    pp->nsubids = par->cursubid;
#ifdef DEBUG
    print("start: %d\n", par->andp->first-pp->firstinst);
    dump(pp);
#endif
out:
    if (par->error) {
        c_free(pp);
        pp = NULL;
    }
    return pp;
}


static int
_runematch(_Rune s, _Rune r, bool icase)
{
    int inv = 0;
    switch (s) {
    case ASC_D: inv = 1; /* fallthrough */
    case ASC_d: return inv ^ (isdigit(r) != 0);
    case ASC_S: inv = 1;
    case ASC_s: return inv ^ (isspace(r) != 0);
    case ASC_W: inv = 1;
    case ASC_w: return inv ^ ((isalnum(r) != 0) | (r == '_'));
    case ASC_AL: inv = 1;
    case ASC_al: return inv ^ (isalpha(r) != 0);
    case ASC_AN: inv = 1;
    case ASC_an: return inv ^ (isalnum(r) != 0);
    case ASC_AS: return (r >= 128);
    case ASC_as: return (r < 128);
    case ASC_BL: inv = 1;
    case ASC_bl: return inv ^ ((r == ' ') | (r == '\t'));
    case ASC_CT: inv = 1;
    case ASC_ct: return inv ^ (iscntrl(r) != 0);
    case ASC_GR: inv = 1;
    case ASC_gr: return inv ^ (isgraph(r) != 0);
    case ASC_PR: inv = 1;
    case ASC_pr: return inv ^ (isprint(r) != 0);
    case ASC_PU: inv = 1;
    case ASC_pu: return inv ^ (ispunct(r) != 0);
    case ASC_LO: inv = 1;
    case ASC_lo: return inv ^ (islower(r) != 0);
    case ASC_UP: inv = 1;
    case ASC_up: return inv ^ (isupper(r) != 0);
    case ASC_XD: inv = 1;
    case ASC_xd: return inv ^ (isxdigit(r) != 0);

    case UTF_D: inv = 1;
    case UTF_d: return inv ^ (utf8_isdigit(r));
    case UTF_S: inv = 1;
    case UTF_s: return inv ^ utf8_isspace(r);
    case UTF_W: inv = 1;
    case UTF_w: return inv ^ (utf8_isalnum(r) | (r == '_'));
    case UTF_AL: inv = 1;
    case UTF_al: return inv ^ utf8_isalpha(r);
    case UTF_AN: inv = 1;
    case UTF_an: return inv ^ utf8_isalnum(r);
    case UTF_LO: inv = 1;
    case UTF_lo: return inv ^ utf8_islower(r);
    case UTF_UP: inv = 1;
    case UTF_up: return inv ^ utf8_isupper(r);
    case UTF_XD: inv = 1;
    case UTF_xd: return inv ^ utf8_isxdigit(r);
    }
    return icase ? utf8_casefold(s) == utf8_casefold(r) : s == r;
}

/*
 *  return 0 if no match
 *        >0 if a match
 *        <0 if we ran out of _relist space
 */
static int
_regexec1(const _Reprog *progp,  /* program to run */
    const char *bol,    /* string to run machine on */
    _Resub *mp,          /* subexpression elements */
    unsigned ms,        /* number of elements at mp */
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
    int i, n, checkstart;
    _Rune r, *rp, *ep;
    int match = 0;

    bool icase = progp->flags.caseless;
    checkstart = j->starttype;
    if (mp)
        for (i=0; i<ms; i++) {
            mp[i].str = NULL;
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
            case RE_IRUNE:
                p = utfruneicase(s, j->startchar);
                goto next1;
            case RE_RUNE:
                p = utfrune(s, j->startchar);
                next1:
                if (p == NULL || s == j->eol)
                    return match;
                s = p;
                break;
            case RE_BOL:
                if (s == bol)
                    break;
                p = utfrune(s, '\n');
                if (p == NULL || s == j->eol)
                    return match;
                s = p+1;
                break;
            }
        }
        n = chartorune(&r, s);

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
                case RE_RUNE:
                case RE_IRUNE:    /* regular character */
                    ok = _runematch(inst->r.rune, r, (icase = inst->type==RE_IRUNE));
                    break;
                case RE_LBRA:
                    tlp->se.m[inst->r.subid].str = s;
                    continue;
                case RE_RBRA:
                    tlp->se.m[inst->r.subid].size = s - tlp->se.m[inst->r.subid].str;
                    continue;
                case RE_ANY:
                    ok = (r != '\n');
                    break;
                case RE_ANYNL:
                    ok = true;
                    break;
                case RE_BOL:
                    if (s == bol || s[-1] == '\n') continue;
                    break;
                case RE_BOS:
                    if (s == bol) continue;
                    break;
                case RE_EOL:
                    if (r == '\n') continue;
                case RE_EOS: /* fallthrough */
                    if (s == j->eol || r == 0) continue;
                    break;
                case RE_EOZ:
                    if (s == j->eol || r == 0 || (r == '\n' && s[1] == 0)) continue;
                    break;
                case RE_NWBOUND:
                    ok = true;
                case RE_WBOUND: /* fallthrough */
                    if (ok ^ (s == bol || s == j->eol || ((utf8_isalnum(utf8_peek_off(s, -1)) || s[-1] == '_')
                                                        ^ (utf8_isalnum(utf8_peek(s)) || s[0] == '_'))))
                        continue;
                    break;
                case RE_NCCLASS:
                    ok = true;
                case RE_CCLASS: /* fallthrough */
                    ep = inst->r.classp->end;
                    for (rp = inst->r.classp->spans; rp < ep; rp += 2) {
                        if ((r >= rp[0] && r <= rp[1]) || (rp[0] == rp[1] && _runematch(rp[0], r, icase)))
                            break;
                    }
                    ok ^= (rp < ep);
                    break;
                case RE_OR:
                    /* evaluate right choice later */
                    if (_renewthread(tlp, inst->r.right, ms, &tlp->se) == tle)
                        return -1;
                    /* efficiency: advance and re-evaluate */
                    continue;
                case RE_END:    /* Match! */
                    match = !(mflags & cre_m_fullmatch) ||
                            ((s == j->eol || r == 0 || r == '\n') &&
                            (tlp->se.m[0].str == bol || tlp->se.m[0].str[-1] == '\n'));
                    tlp->se.m[0].size = s - tlp->se.m[0].str;
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
    _Resub *mp,          /* subexpression elements */
    unsigned ms,        /* number of elements at mp */
    _Reljunk *j,
    int mflags
)
{
    int rv;
    _Relist *relists;

    /* mark space */
    relists = (_Relist *)c_malloc(2 * _BIGLISTSIZE*sizeof(_Relist));
    if (relists == NULL)
        return -1;

    j->relist[0] = relists;
    j->relist[1] = relists + _BIGLISTSIZE;
    j->reliste[0] = relists + _BIGLISTSIZE - 2;
    j->reliste[1] = relists + 2*_BIGLISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, j, mflags);
    c_free(relists);
    return rv;
}

static int
_regexec(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    unsigned ms,        /* number of elements at mp */
    _Resub mp[],         /* subexpression elements */
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
        if (mflags & cre_m_startend)
            j.starts = mp[0].str, j.eol = mp[0].str + mp[0].size;
        else if (mflags & cre_m_next)
            j.starts = mp[0].str + mp[0].size;
    }

    j.starttype = 0;
    j.startchar = 0;
    int rune_type = progp->flags.caseless ? RE_IRUNE : RE_RUNE;
    if (progp->startinst->type == rune_type && progp->startinst->r.rune < 128) {
        j.starttype = rune_type;
        j.startchar = progp->startinst->r.rune;
    }
    if (progp->startinst->type == RE_BOL)
        j.starttype = RE_BOL;

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
_build_subst(const char* replace, unsigned nmatch, const csview match[],
             bool (*mfun)(int, csview, cstr*), cstr* subst) {
    cstr_buf buf = cstr_buffer(subst);
    unsigned len = 0, cap = buf.cap;
    char* dst = buf.data;
    cstr mstr = cstr_null;

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
                        dst = cstr_reserve(subst, cap = cap*3/2 + m.size);
                    for (unsigned i = 0; i < m.size; ++i)
                        dst[len++] = m.str[i];
                }
                ++replace;
            case '\0':
                continue;
            }
        }
        if (len == cap)
            dst = cstr_reserve(subst, cap = cap*3/2 + 4);
        dst[len++] = *replace++;
    }
    cstr_drop(&mstr);
    _cstr_set_size(subst, len);
}


/* ---------------------------------------------------------------
 * API functions
 */

int 
cregex_compile(cregex *self, const char* pattern, int cflags) {
    _Parser par;
    self->prog = _regcomp1(self->prog, &par, pattern, cflags);
    return self->error = par.error;
}

int
cregex_captures(const cregex* self) {
    return self->prog ? 1 + self->prog->nsubids : 0;
}

int
cregex_find(const cregex* re, const char* input,
            csview match[], int mflags) {
    int res = _regexec(re->prog, input, cregex_captures(re), match, mflags);
    switch (res) {
    case 1: return cre_success;
    case 0: return cre_nomatch;
    default: return cre_matcherror;
    }
}

int
cregex_find_pattern(const char* pattern, const char* input,
                    csview match[], int cmflags) {
    cregex re = cregex_init();
    int res = cregex_compile(&re, pattern, cmflags);
    if (res != cre_success) return res;
    res = cregex_find(&re, input, match, cmflags);
    cregex_drop(&re);
    return res;
}

cstr
cregex_replace_sv(const cregex* re, csview input, const char* replace, unsigned count,
                  bool (*mfun)(int, csview, cstr*), int rflags) {
    cstr out = cstr_null;
    cstr subst = cstr_null;
    csview match[cre_MAXCAPTURES];
    unsigned nmatch = cregex_captures(re);
    if (!count) count = ~0;
    bool copy = !(rflags & cre_r_strip);

    while (count-- && cregex_find_sv(re, input, match) == cre_success) {
        _build_subst(replace, nmatch, match, mfun, &subst);
        const size_t mpos = match[0].str - input.str;
        if (copy & (mpos > 0)) cstr_append_n(&out, input.str, mpos);
        cstr_append_s(&out, subst);
        input.str = match[0].str + match[0].size;
        input.size -= mpos + match[0].size;
    }
    if (copy) cstr_append_sv(&out, input);
    cstr_drop(&subst);
    return out;
}

cstr
cregex_replace_pattern(const char* pattern, const char* input, const char* replace, unsigned count,
                       bool (*mfun)(int, csview, cstr*), int crflags) {
    cregex re = cregex_init();
    if (cregex_compile(&re, pattern, crflags) != cre_success)
        return cstr_new("[[error: invalid regex pattern]]");
    csview sv = {input, strlen(input)};
    cstr out = cregex_replace_sv(&re, sv, replace, count, mfun, crflags);
    cregex_drop(&re);
    return out;
}

void
cregex_drop(cregex* self) {
    c_free(self->prog);
}
