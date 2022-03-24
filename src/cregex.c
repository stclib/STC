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

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <stc/cregex.h>
#include "cregex_utf8.c"

typedef uint32_t Rune; /* Utf8 code point */
typedef int32_t Token;
/* max character classes per program */
#define NCLASS creg_max_classes
/* max subexpressions */
#define NSUBEXP creg_max_captures
/* max rune ranges per character class */
#define NCCRUNE (NSUBEXP * 2)

/*
 *    character class, each pair of rune's defines a range
 */
typedef struct
{
    Rune *end;
    Rune spans[NCCRUNE];
} Reclass;

/*
 *    Machine instructions
 */
typedef struct Reinst
{
    Token type;
    union {
        Reclass *classp;        /* class pointer */
        Rune    rune;           /* character */
        int     subid;          /* sub-expression id for RBRA and LBRA */
        struct Reinst *right;   /* right child of OR */
    } r;
    union {    /* regexp relies on these two being in the same union */
        struct Reinst *left;    /* left child of OR */
        struct Reinst *next;    /* next instruction for CAT & LBRA */
    } l;
} Reinst;

typedef struct {
    bool caseless;
    bool dotall;
} Reflags;

/*
 *    Reprogram definition
 */
typedef struct Reprog
{
    Reinst  *startinst;     /* start pc */
    Reflags flags;
    int nsubids;
    Reclass cclass[NCLASS]; /* .data */
    Reinst  firstinst[];    /* .text : originally 5 elements? */
} Reprog;

/*
 *    Sub expression matches
 */
typedef cregmatch Resub;

/*
 *  substitution list
 */
typedef struct Resublist
{
    Resub m[NSUBEXP];
} Resublist;

/*
 * Actions and Tokens (Reinst types)
 *
 *  0x800000-0x80FFFF: operators, value => precedence
 *  0x810000-0x81FFFF: RUNE and char classes.
 *  0x820000-0x82FFFF: tokens, i.e. operands for operators
 */
enum {
    MASK        = 0xFF00000,
    OPERATOR    = 0x8000000, /* Bitmask of all operators */
    START       = 0x8000001, /* Start, used for marker on stack */
    RBRA        ,           /* Right bracket, ) */
    LBRA        ,           /* Left bracket, ( */
    OR          ,           /* Alternation, | */
    CAT         ,           /* Concatentation, implicit operator */
    STAR        ,           /* Closure, * */
    PLUS        ,           /* a+ == aa* */
    QUEST       ,           /* a? == a|nothing, i.e. 0 or 1 a's */
    RUNE        = 0x8100000,
    IRUNE,
    ASC_bl      , ASC_BL,   /* blank */
    ASC_ct      , ASC_CT,   /* ctrl */
    ASC_gr      , ASC_GR,   /* graphic */
    ASC_pr      , ASC_PR,   /* print */
    ASC_pt      , ASC_PT,   /* punct */
    U8_Nd       , U8N_Nd,    /* dec digit, non-digit */
    U8_LC       , U8N_LC,   /* utf8 letter cased */
    U8_Ll       , U8N_Ll,   /* utf8 letter lower */
    U8_Lu       , U8N_Lu,   /* utf8 letter upper */
    U8_Zs       , U8N_Zs,   /* utf8 white space */
    U8_Xnx      , U8N_Xnx,  /* utf8 hex digit */
    U8_Xan      , U8N_Xan,  /* utf8 alphanumeric */
    U8_Xw       , U8N_Xw,   /* utf8 word */
    ANY         = 0x8200000, /* Any character except newline, . */
    ANYNL       ,           /* Any character including newline, . */
    NOP         ,           /* No operation, internal use only */
    BOL         , BOS,      /* Beginning of line, string, ^ */
    EOL         , EOS, EOZ, /* End of line, string, $ */
    CCLASS      ,           /* Character class, [] */
    NCCLASS     ,           /* Negated character class, [] */
    WBOUND      ,           /* Non-word boundary, not consuming meta char */
    NWBOUND     ,           /* Word boundary, not consuming meta char */
    END         = 0x82FFFFF, /* Terminate: match found */
};

/*
 *  regexec execution lists
 */
#define LISTSIZE    10
#define BIGLISTSIZE (10*LISTSIZE)

typedef struct Relist
{
    Reinst*     inst;       /* Reinstruction of the thread */
    Resublist   se;         /* matched subexpressions in this thread */
} Relist;

typedef struct Reljunk
{
    Relist*     relist[2];
    Relist*     reliste[2];
    int         starttype;
    Rune        startchar;
    const char* starts;
    const char* eol;
} Reljunk;

/*
 * utf8 and Rune code
 */

static int
chartorune(Rune *rune, const char *s)
{
    utf8_decode_t ctx = {UTF8_OK};
    const uint8_t *b = (const uint8_t*)s;
    utf8_decode(&ctx, *b++);
    switch (ctx.size) {
    case 4: utf8_decode(&ctx, *b++);
    case 3: utf8_decode(&ctx, *b++);
    case 2: utf8_decode(&ctx, *b++);
    }
    *rune = ctx.codep;
    return ctx.size;
}

static const char*
utfrune(const char *s, Rune c)
{
    Rune r;

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
utfruneicase(const char *s, Rune c)
{
    Rune r;
    c = utf8_tolower(c);
    for (;;) {
        int n = chartorune(&r, s);
        if (utf8_tolower(r) == c) return s;
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
_renewmatch(Resub *mp, int ms, Resublist *sp, int nsubids)
{
    int i;

    if (mp==NULL || ms<=0)
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
static Relist*
_renewthread(Relist *lp,  /* _relist to add to */
    Reinst *ip,           /* instruction to add */
    int ms,
    Resublist *sep)       /* pointers to subexpressions */
{
    Relist *p;

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
static Relist*
_renewemptythread(Relist *lp,   /* _relist to add to */
    Reinst *ip,                 /* instruction to add */
    int ms,
    const char *sp)             /* pointers to subexpressions */
{
    Relist *p;

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
 * Parser Information
 */
typedef struct Node
{
    Reinst*    first;
    Reinst*    last;
} Node;

#define NSTACK 20
typedef struct Parser
{
    const char* exprp;   /* pointer to next character in source expression */
    Node andstack[NSTACK];
    Node* andp;
    Token atorstack[NSTACK];
    Token* atorp;
    short subidstack[NSTACK]; /* parallel to atorstack */
    short* subidp;
    short cursubid;      /* id of current subexpression */
    int errors;
    Reflags flags;
    int dot_type;
    int rune_type;
    bool litmode;
    bool lastwasand;     /* Last token was operand */
    bool lexdone;
    short nbra;
    short nclass;
    Rune yyrune;         /* last lex'd rune */
    Reclass *yyclassp;   /* last lex'd class */
    Reclass* classp;
    Reinst* freep;
    jmp_buf regkaboom;
} Parser;

/* predeclared crap */
static void _operator(Parser *par, Token type);
static void pushand(Parser *par, Reinst *first, Reinst *last);
static void pushator(Parser *par, Token type);
static void evaluntil(Parser *par, Token type);
static int  bldcclass(Parser *par);

static void
rcerror(Parser *par, cregex_error_t err)
{
    par->errors = err;
    longjmp(par->regkaboom, 1);
}

static Reinst*
newinst(Parser *par, Token t)
{
    par->freep->type = t;
    par->freep->l.left = 0;
    par->freep->r.right = 0;
    return par->freep++;
}

static void
operand(Parser *par, Token t)
{
    Reinst *i;

    if (par->lastwasand)
        _operator(par, CAT);    /* catenate is implicit */
    i = newinst(par, t);

    if ((t == CCLASS) | (t == NCCLASS))
        i->r.classp = par->yyclassp;
    if ((t == RUNE) | (t == IRUNE))
        i->r.rune = par->yyrune;

    pushand(par, i, i);
    par->lastwasand = true;
}

static void
_operator(Parser *par, Token t)
{
    if (t==RBRA && --par->nbra<0)
        rcerror(par, creg_unmatchedrightparenthesis);
    if (t==LBRA) {
        if (++par->cursubid >= NSUBEXP)
            rcerror(par, creg_toomanysubexpressions);
        par->nbra++;
        if (par->lastwasand)
            _operator(par, CAT);
    } else
        evaluntil(par, t);
    if (t != RBRA)
        pushator(par, t);
    par->lastwasand = 0;
    if (t==STAR || t==QUEST || t==PLUS || t==RBRA)
        par->lastwasand = true;    /* these look like operands */
}

static void
pushand(Parser *par, Reinst *f, Reinst *l)
{
    if (par->andp >= &par->andstack[NSTACK])
        rcerror(par, creg_operandstackoverflow);
    par->andp->first = f;
    par->andp->last = l;
    par->andp++;
}

static void
pushator(Parser *par, Token t)
{
    if (par->atorp >= &par->atorstack[NSTACK])
        rcerror(par, creg_operatorstackoverflow);
    *par->atorp++ = t;
    *par->subidp++ = par->cursubid;
}

static Node*
popand(Parser *par, Token op)
{
    Reinst *inst;

    if (par->andp <= &par->andstack[0]) {
        rcerror(par, creg_missingoperand);
        inst = newinst(par, NOP);
        pushand(par, inst, inst);
    }
    return --par->andp;
}

static Token
popator(Parser *par)
{
    if (par->atorp <= &par->atorstack[0])
        rcerror(par, creg_operatorstackunderflow);
    --par->subidp;
    return *--par->atorp;
}

static void
evaluntil(Parser *par, Token pri)
{
    Node *op1, *op2;
    Reinst *inst1, *inst2;

    while (pri==RBRA || par->atorp[-1]>=pri) {
        switch (popator(par)) {
        default:
            rcerror(par, creg_unknownoperator);
            break;
        case LBRA:        /* must have been RBRA */
            op1 = popand(par, '(');
            inst2 = newinst(par, RBRA);
            inst2->r.subid = *par->subidp;
            op1->last->l.next = inst2;
            inst1 = newinst(par, LBRA);
            inst1->r.subid = *par->subidp;
            inst1->l.next = op1->first;
            pushand(par, inst1, inst2);
            return;
        case OR:
            op2 = popand(par, '|');
            op1 = popand(par, '|');
            inst2 = newinst(par, NOP);
            op2->last->l.next = inst2;
            op1->last->l.next = inst2;
            inst1 = newinst(par, OR);
            inst1->r.right = op1->first;
            inst1->l.left = op2->first;
            pushand(par, inst1, inst2);
            break;
        case CAT:
            op2 = popand(par, 0);
            op1 = popand(par, 0);
            op1->last->l.next = op2->first;
            pushand(par, op1->first, op2->last);
            break;
        case STAR:
            op2 = popand(par, '*');
            inst1 = newinst(par, OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            pushand(par, inst1, inst1);
            break;
        case PLUS:
            op2 = popand(par, '+');
            inst1 = newinst(par, OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            pushand(par, op2->first, inst1);
            break;
        case QUEST:
            op2 = popand(par, '?');
            inst1 = newinst(par, OR);
            inst2 = newinst(par, NOP);
            inst1->l.left = inst2;
            inst1->r.right = op2->first;
            op2->last->l.next = inst2;
            pushand(par, inst1, inst2);
            break;
        }
    }
}

static Reprog*
optimize(Parser *par, Reprog *pp)
{
    Reinst *inst, *target;
    size_t size;
    Reprog *npp;
    Reclass *cl;
    ptrdiff_t diff;

    /*
     *  get rid of NOOP chains
     */
    for (inst = pp->firstinst; inst->type != END; inst++) {
        target = inst->l.next;
        while (target->type == NOP)
            target = target->l.next;
        inst->l.next = target;
    }

    /*
     *  The original allocation is for an area larger than
     *  necessary.  Reallocate to the actual space used
     *  and then relocate the code.
     */
    size = sizeof(Reprog) + (par->freep - pp->firstinst)*sizeof(Reinst);
    npp = (Reprog *)realloc(pp, size);
    if (npp==NULL || npp==pp)
        return pp;
    diff = (char *)npp - (char *)pp;
    par->freep = (Reinst *)((char *)par->freep + diff);
    for (inst = npp->firstinst; inst < par->freep; inst++) {
        switch (inst->type) {
        case OR:
        case STAR:
        case PLUS:
        case QUEST:
            inst->r.right = (Reinst *)((char*)inst->r.right + diff);
            break;
        case CCLASS:
        case NCCLASS:
            inst->r.right = (Reinst *)((char*)inst->r.right + diff);
            cl = inst->r.classp;
            cl->end = (Rune *)((char*)cl->end + diff);
            break;
        }
        inst->l.left = (Reinst *)((char*)inst->l.left + diff);
    }
    npp->startinst = (Reinst *)((char*)npp->startinst + diff);
    return npp;
}

static Reclass*
newclass(Parser *par)
{
    if (par->nclass >= NCLASS)
        rcerror(par, creg_toomanycharacterclasses);
    return &(par->classp[par->nclass++]);
}

static int
nextc(Parser *par, Rune *rp)
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
            case 'd': *rp = U8_Nd; break;
            case 'D': *rp = U8N_Nd; break;
            case 's': *rp = U8_Zs; break;
            case 'S': *rp = U8N_Zs; break;
            case 'w': *rp = U8_Xw; break;
            case 'W': *rp = U8N_Xw; break;
            case 'x': if (*par->exprp != '{') break;
                *rp = 0; sscanf(++par->exprp, "%x", rp);
                while (*par->exprp) if (*(par->exprp++) == '}') break;
                if (par->exprp[-1] != '}')
                    rcerror(par, creg_unmatchedrightparenthesis);
                return 2;
            case 'p': case 'P': { /* https://www.regular-expressions.info/unicode.html */
                static struct { const char* c; int n, r; } cls[] = {
                    {"{Alpha}", 7, U8_LC}, {"{LC}", 4, U8_LC}, 
                    {"{Alnum}", 7, U8_Xan},
                    {"{Digit}", 7, U8_Nd}, {"{Nd}", 4, U8_Nd},
                    {"{Lower}", 7, U8_Ll}, {"{Ll}", 4, U8_Ll},
                    {"{Space}", 7, U8_Zs}, {"{Zs}", 4, U8_Zs},
                    {"{Upper}", 7, U8_Lu}, {"{Lu}", 4, U8_Lu},
                    {"{XDigit}", 8, U8_Xnx},
                    {"{Blank}", 7, ASC_bl},
                    {"{Graph}", 7, ASC_gr},
                    {"{Print}", 7, ASC_pr},
                    {"{Punct}", 7, ASC_pt},
                };
                int inv = *rp == 'P';
                for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i)
                    if (!strncmp(par->exprp, cls[i].c, cls[i].n)) {
                        if (par->rune_type == IRUNE && (cls[i].r == U8_Ll || cls[i].r == U8_Lu))
                            *rp = U8_LC + inv;
                        else
                            *rp = cls[i].r + inv;
                        par->exprp += cls[i].n;
                        break;
                    }
                if (*rp < OPERATOR) {
                    rcerror(par, creg_unknownoperator);
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

static Token
lex(Parser *par)
{
    int quoted;
    start:
    quoted = nextc(par, &par->yyrune);
    if (quoted) {
        if (quoted == 2) {
            if (par->litmode && par->yyrune == 'E') {
                par->litmode = false;
                goto start;
            }
            return par->yyrune == 0 ? END : par->rune_type;
        }
        switch (par->yyrune) {
        case  0 : return END;
        case 'b': return WBOUND;
        case 'B': return NWBOUND;
        case 'A': return BOS;
        case 'z': return EOS;
        case 'Z': return EOZ;
        case 'Q': par->litmode = true;
                  goto start;
        default : return par->rune_type;
        }
    }

    switch (par->yyrune) {
    case  0 : return END;
    case '*': return STAR;
    case '?': return QUEST;
    case '+': return PLUS;
    case '|': return OR;
    case '.': return par->dot_type;
    case '(':
        if (par->exprp[0] == '?') {
            for (int k = 1, enable = 1; ; ++k) switch (par->exprp[k]) {
                case  0 : par->exprp += k; return END;
                case ')': par->exprp += k + 1; goto start;
                case '-': enable = 0; break;
                case 's': if (!par->flags.dotall) par->dot_type = ANY + enable; break;
                case 'i': if (!par->flags.caseless) par->rune_type = RUNE + enable; break;
                default: rcerror(par, creg_unknownoperator); return 0;
            }
        }
        return LBRA;
    case ')': return RBRA;
    case '^': return BOL;
    case '$': return EOL;
    case '[': return bldcclass(par);
    }
    return par->rune_type;
}

static Token
bldcclass(Parser *par)
{
    Token type;
    Rune r[NCCRUNE];
    Rune *p, *ep, *np;
    Rune rune;
    int quoted;

    /* we have already seen the '[' */
    type = CCLASS;
    par->yyclassp = newclass(par);

    /* look ahead for negation */
    /* SPECIAL CASE!!! negated classes don't match \n */
    ep = r;
    quoted = nextc(par, &rune);
    if (!quoted && rune == '^') {
        type = NCCLASS;
        quoted = nextc(par, &rune);
        *ep++ = '\n';
        *ep++ = '\n';
    }

    /* parse class into a set of spans */
    for (; ep < &r[NCCRUNE]; quoted = nextc(par, &rune)) {
        if (rune == 0) {
            rcerror(par, creg_malformedcharacterclass);
            return 0;
        }
        if (!quoted) {
            if (rune == ']')
                break;
            if (rune == '-') {
                if (ep != r && *par->exprp != ']') {
                    quoted = nextc(par, &rune);
                    if (rune == 0) {
                        rcerror(par, creg_malformedcharacterclass);
                        return 0;
                    }
                    ep[-1] = rune;
                    continue;
                }
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

static Reprog*
regcomp1(Parser *par, const char *s, int cflags)
{
    Token token;
    Reprog *volatile pp;

    /* get memory for the program. estimated max usage */
    const int instcap = 5 + 6*strlen(s);
    pp = (Reprog *)malloc(sizeof(Reprog) + instcap*sizeof(Reinst));
    if (pp == NULL) {
        rcerror(par, creg_outofmemory);
        return NULL;
    }
    pp->flags.caseless = (cflags & creg_caseless) != 0;
    pp->flags.dotall = (cflags & creg_dotall) != 0;
    par->freep = pp->firstinst;
    par->classp = pp->cclass;
    par->errors = 0;

    if (setjmp(par->regkaboom))
        goto out;

    /* go compile the sucker */
    par->lexdone = false;
    par->flags = pp->flags;
    par->rune_type = pp->flags.caseless ? IRUNE : RUNE;
    par->dot_type = pp->flags.dotall ? ANYNL : ANY;
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
    pushator(par, START-1);
    while ((token = lex(par)) != END) {
        if ((token & MASK) == OPERATOR)
            _operator(par, token);
        else
            operand(par, token);
    }

    /* Close with a low priority operator */
    evaluntil(par, START);

    /* Force END */
    operand(par, END);
    evaluntil(par, START);
#ifdef DEBUG
    dumpstack(par);
#endif
    if (par->nbra)
        rcerror(par, creg_unmatchedleftparenthesis);
    --par->andp;    /* points to first and only operand */
    pp->startinst = par->andp->first;
#ifdef DEBUG
    dump(pp);
#endif
    pp = optimize(par, pp);
    pp->nsubids = par->cursubid;
#ifdef DEBUG
    print("start: %d\n", par->andp->first-pp->firstinst);
    dump(pp);
#endif
out:
    if (par->errors) {
        free(pp);
        pp = NULL;
    }
    return pp;
}


static int
runematch(Rune s, Rune r, bool icase)
{
    int inv = 0;
    switch (s) {
    case ASC_BL: inv = 1; /* fallthrough */
    case ASC_bl: return inv ^ ((r == ' ') | (r == '\t'));
    case ASC_CT: inv = 1;
    case ASC_ct: return inv ^ (iscntrl(r) != 0);
    case ASC_GR: inv = 1;
    case ASC_gr: return inv ^ (isgraph(r) != 0);
    case ASC_PR: inv = 1;
    case ASC_pr: return inv ^ (isprint(r) != 0);
    case ASC_PT: inv = 1;
    case ASC_pt: return inv ^ (ispunct(r) != 0);
    case U8N_Nd: inv = 1;
    case U8_Nd: return inv ^ (utf8_isdigit(r));
    case U8N_LC: inv = 1;
    case U8_LC: return inv ^ utf8_isalpha(r);
    case U8N_Ll: inv = 1;
    case U8_Ll: return inv ^ utf8_islower(r);
    case U8N_Lu: inv = 1;
    case U8_Lu: return inv ^ utf8_isupper(r);
    case U8N_Zs: inv = 1;
    case U8_Zs: return inv ^ utf8_isspace(r);
    case U8N_Xan: inv = 1;
    case U8_Xan: return inv ^ utf8_isalnum(r);
    case U8N_Xnx: inv = 1;
    case U8_Xnx: return inv ^ utf8_isxdigit(r);
    case U8N_Xw: inv = 1;
    case U8_Xw: return inv ^ (utf8_isalnum(r) | (r == '_'));
    }
    return icase ? utf8_tolower(s) == utf8_tolower(r) : s == r;
}

/*
 *  return 0 if no match
 *        >0 if a match
 *        <0 if we ran out of _relist space
 */
static int
regexec1(const Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    Resub *mp,    /* subexpression elements */
    int ms,        /* number of elements at mp */
    Reljunk *j,
    int mflags
)
{
    int flag=0;
    Reinst *inst;
    Relist *tlp;
    Relist *tl, *nl;    /* This list, next list */
    Relist *tle, *nle;  /* Ends of this and next list */
    const char *s, *p;
    int i, n, checkstart;
    Rune r, *rp, *ep;
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
            case IRUNE:
                p = utfruneicase(s, j->startchar);
                goto next1;
            case RUNE:
                p = utfrune(s, j->startchar);
                next1:
                if (p == NULL || s == j->eol)
                    return match;
                s = p;
                break;
            case BOL:
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
                case RUNE:
                case IRUNE:    /* regular character */
                    ok = runematch(inst->r.rune, r, (icase = inst->type==IRUNE));
                    break;
                case LBRA:
                    tlp->se.m[inst->r.subid].str = s;
                    continue;
                case RBRA:
                    tlp->se.m[inst->r.subid].size = s - tlp->se.m[inst->r.subid].str;
                    continue;
                case ANY:
                    ok = (r != '\n');
                    break;
                case ANYNL:
                    ok = true;
                    break;
                case BOL:
                    if (s == bol || s[-1] == '\n') continue;
                    break;
                case BOS:
                    if (s == bol) continue;
                    break;
                case EOL:
                    if (r == '\n') continue;
                case EOS: /* fallthrough */
                    if (s == j->eol || r == 0) continue;
                    break;
                case EOZ:
                    if (s == j->eol || r == 0 || (r == '\n' && s[1] == 0)) continue;
                    break;
                case NWBOUND:
                    ok = true;
                case WBOUND: /* fallthrough */
                    if (ok ^ (s == bol || s == j->eol || ((utf8_isalnum(s[-1]) || s[-1] == '_')
                                                        ^ (utf8_isalnum(s[ 0]) || s[ 0] == '_'))))
                        continue;
                    break;
                case NCCLASS:
                    ok = true;
                case CCLASS: /* fallthrough */
                    ep = inst->r.classp->end;
                    for (rp = inst->r.classp->spans; rp < ep; rp += 2) {
                        if ((r >= rp[0] && r <= rp[1]) || (rp[0] == rp[1] && runematch(rp[0], r, icase)))
                            break;
                    }
                    ok ^= (rp < ep);
                    break;
                case OR:
                    /* evaluate right choice later */
                    if (_renewthread(tlp, inst->r.right, ms, &tlp->se) == tle)
                        return -1;
                    /* efficiency: advance and re-evaluate */
                    continue;
                case END:    /* Match! */
                    match = !(mflags & creg_fullmatch) ||
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
regexec2(const Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    Resub *mp,    /* subexpression elements */
    int ms,        /* number of elements at mp */
    Reljunk *j,
    int mflags
)
{
    int rv;
    Relist *relists;

    /* mark space */
    relists = (Relist *)malloc(2 * BIGLISTSIZE*sizeof(Relist));
    if (relists == NULL)
        return -1;

    j->relist[0] = relists;
    j->relist[1] = relists + BIGLISTSIZE;
    j->reliste[0] = relists + BIGLISTSIZE - 2;
    j->reliste[1] = relists + 2*BIGLISTSIZE - 2;

    rv = regexec1(progp, bol, mp, ms, j, mflags);
    free(relists);
    return rv;
}

static int
regexec9(const Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    int ms,             /* number of elements at mp */
    Resub mp[],         /* subexpression elements */
    int mflags)
{
    Reljunk j;
    Relist relist0[LISTSIZE], relist1[LISTSIZE];
    int rv;

    /*
      *  use user-specified starting/ending location if specified
     */
    j.starts = bol;
    j.eol = NULL;

    if (mp && mp->str && ms>0) {
        if (mflags & creg_startend)
            j.starts = mp->str, j.eol = mp->str + mp->size;
        else if (mflags & creg_next)
            j.starts = mp->str + mp->size;
    }

    j.starttype = 0;
    j.startchar = 0;
    int rune_type = progp->flags.caseless ? IRUNE : RUNE;
    if (progp->startinst->type == rune_type && progp->startinst->r.rune < 128) {
        j.starttype = rune_type;
        j.startchar = progp->startinst->r.rune;
    }
    if (progp->startinst->type == BOL)
        j.starttype = BOL;

    /* mark space */
    j.relist[0] = relist0;
    j.relist[1] = relist1;
    j.reliste[0] = relist0 + LISTSIZE - 2;
    j.reliste[1] = relist1 + LISTSIZE - 2;

    rv = regexec1(progp, bol, mp, ms, &j, mflags);
    if (rv >= 0)
        return rv;
    rv = regexec2(progp, bol, mp, ms, &j, mflags);
    return rv;
}

/*
 * API functions
 */

/* substitute into one string using the matches from the last regexec() */
void cregex_replace(
    const char *sp,     /* source string */
    char *dp,           /* destination string */
    int dlen,
    int ms,             /* number of elements pointed to by mp */
    const cregmatch mp[])   /* subexpression elements */
{
    const char *ssp, *ep;
    int i;

    ep = dp+dlen-1;
    while (*sp != '\0') {
        if (*sp == '\\') {
            switch (*++sp) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                i = *sp - '0';
                if (mp[i].str != NULL && mp != NULL && ms > i)
                    for (ssp = mp[i].str; ssp < (mp[i].str + mp[i].size); ssp++)
                        if (dp < ep)
                            *dp++ = *ssp;
                break;
            case '\\':
                if (dp < ep)
                    *dp++ = '\\';
                break;
            case '\0':
                sp--;
                break;
            default:
                if (dp < ep)
                    *dp++ = *sp;
                break;
            }
        } else if (*sp == '&') {
            if (mp[0].str != NULL && mp != NULL && ms > 0)
                for (ssp = mp[0].str; ssp < (mp[0].str + mp[0].size); ssp++)
                    if (dp < ep)
                        *dp++ = *ssp;
        } else {
            if (dp < ep)
                *dp++ = *sp;
        }
        sp++;
    }
    *dp = '\0';
}

int cregex_compile(cregex *rx, const char* pattern, int cflags) {
    Parser par;
    rx->prog = regcomp1(&par, pattern, cflags);
    if (rx->prog)
        return 1 + rx->prog->nsubids;
    return par.errors;
}

int cregex_captures(cregex rx) {
    return rx.prog ? 1 + rx.prog->nsubids : 0;
}

int cregex_find(const cregex *rx, const char* string,
                size_t nmatch, cregmatch match[], int mflags) {
    int res = regexec9(rx->prog, string, nmatch, match, mflags);
    switch (res) {
    case 1: return 1 + rx->prog->nsubids;
    case 0: return creg_nomatch;
    default: return creg_matcherror;
    }
}

void cregex_drop(cregex* self) {
    free(self->prog);
}
