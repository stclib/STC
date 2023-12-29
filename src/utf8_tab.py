#!python
# To generate "include/stc/priv/utf8_tab.c" file.

import pandas as pd
import numpy as np

_UNICODE_DIR = "https://www.unicode.org/Public/15.0.0/ucd"


def read_unidata(casetype='lowcase', category='Lu', bitrange=16):
    df = pd.read_csv(_UNICODE_DIR+'/UnicodeData.txt', sep=';', converters={0: lambda x: int(x, base=16)},
                      names=['code', 'name', 'category', 'canclass', 'bidircat', 'chrdecomp',
                             'decdig', 'digval', 'numval', 'mirrored', 'uc1name', 'comment',
                             'upcase', 'lowcase', 'titlecase'],
                      usecols=['code', 'name', 'category', 'bidircat', 'upcase', 'lowcase', 'titlecase'])
    if bitrange == 16:
        df = df[df['code'] < (1<<16)]
    else:
        df = df[df['code'] >= (1<<16)]

    if category:
        df = df[df['category'] == category]
    df = df.replace(np.nan, '0')
    for k in ['upcase', 'lowcase', 'titlecase']:
        df[k] = df[k].apply(int, base=16)

    if casetype:                   # 'lowcase', 'upcase', 'titlecase'
        df = df[df[casetype] != 0] # remove mappings to 0
    return df


def read_casefold(bitrange):
    df = pd.read_csv(_UNICODE_DIR+'/CaseFolding.txt', engine='python', sep='; #? ?', comment='#',
                     converters={0: lambda x: int(x, base=16)},
                     names=['code', 'status', 'lowcase', 'name']) # comment => 'name'
    if bitrange == 16:
        df = df[df['code'] < (1<<16)]
    else:
        df = df[df['code'] >= (1<<16)]

    df = df[df.status.isin(['S', 'C'])]
    df['lowcase'] = df['lowcase'].apply(int, base=16)
    return df


def make_caselist(df, casetype):
    caselist=[]
    for idx, row in df.iterrows():
        caselist.append((row['code'], row[casetype], row['name']))
    return caselist


def make_table(caselist):
    prev_a, prev_b = 0, 0
    diff_a, diff_b = 0, 0
    prev_offs = 0
    n_1 = len(caselist) - 1

    table = []
    for j in range(0, len(caselist)):
        a, b, name = caselist[j]
        offset = b - a

        if abs(diff_a) > 2 or a - prev_a != diff_a or b - prev_b != diff_b or prev_offs != offset:
            if  j > 0: # and start_a not in [0xAB70, 0x13F8]: # BUG in CaseFolding.txt V14
                table.append([start_a, prev_a, prev_b, start_name])
            if j < n_1:
                diff_a = caselist[j+1][0] - a
                diff_b = caselist[j+1][1] - b
                start_a = a
                start_name = name

        prev_a, prev_b = a, b
        prev_offs = offset

    table.append((start_a, a, b, start_name))
    return table


def print_table(name, table, style=1, bitrange=16):
    r32 = '32' if bitrange == 32 else ''
    print('#include <stdint.h>\n')
    print('struct CaseMapping%s { uint%d_t c1, c2, m2; };\n' % (r32, bitrange))
    print('static struct CaseMapping%s %s%s[] = {' % (r32, name, r32))
    for a,b,c,t in table:
        if style == 1:   # first char with name
            d = b - a + 1 if abs(c - b) != 1 else (b - a)/2 + 1
            print('    {0x%04X, 0x%04X, 0x%04X}, // %s %s (%2d) %s' % (a, b, c, chr(a), chr(a + c - b), d, t))
        elif style == 2: # all chars
            print('    {0x%04X, 0x%04X, 0x%04X}, // ' % (a, b, c), end='')
            n = 0
            for k in range(a, b+1, 2 if c - b == 1 else 1):
                n += 1
                if n % 17 == 0:
                    print('\n                              // ', end='')
                print('%s %s, ' % (chr(k), chr(k + c - b)), end='')
            print('')
    print('}; // %d\n' % (len(table)))


def print_index_table(name, indtab):
    print('\nstatic uint8_t %s[%d] = {\n   ' % (name, len(indtab)), end='')
    for i in range(len(indtab)):
        print(" %d," % (indtab[i]), end='\n   ' if (i+1) % 20 == 0 else '')
    print('\n};')


def compile_table(casetype='lowcase', category=None, bitrange=16):
    if category:
        df = read_unidata(casetype, category, bitrange)
    else:
        df = read_casefold(bitrange)
    caselist = make_caselist(df, casetype)
    table = make_table(caselist)
    return table


def main():
    bitrange = 16

    casemappings = compile_table('lowcase', None, bitrange) # CaseFolding.txt
    upcase       = compile_table('lowcase', 'Lu', bitrange) # UnicodeData.txt uppercase
    lowcase      = compile_table('upcase', 'Ll', bitrange) # UnicodeData.txt lowercase

    casefolding_len = len(casemappings)

    # add additional Lu => Ll mappings from UnicodeData.txt
    # create upcase_ind: lower => upper index list sorted by mapped lowercase values:
    upcase_ind = []
    for v in upcase:
        try:
            upcase_ind.append(casemappings.index(v))
        except:
            upcase_ind.append(len(casemappings))
            casemappings.append(v)

    # add additional Ll => Lu mappings from UnicodeData.txt
    # create lowcase_ind: upper => lower index list sorted by uppercase values:
    lowcase_ind = []
    for u in lowcase:
        v = (u[2] - (u[1] - u[0]), u[2], u[1], '')
        try:
            j = next(i for i,x in enumerate(casemappings) if x[0]==v[0] and x[1]==v[1] and x[2]==v[2])
            lowcase_ind.append(j)
        except:
            lowcase_ind.append(len(casemappings))
            casemappings.append(v)

    print_table('casemappings', casemappings, style=1, bitrange=bitrange)
    print('enum { casefold_len = %d };' % casefolding_len)

    # upcase => low
    upcase_ind.sort(key=lambda i: casemappings[i][0])
    print_index_table('upcase_ind', upcase_ind)

    # lowcase => up. add "missing" SHARP S caused by https://www.unicode.org/policies/stability_policy.html#Case_Pair
    if bitrange == 16:
        lowcase_ind.append(next(i for i,x in enumerate(casemappings) if x[0]==ord('ẞ')))
    lowcase_ind.sort(key=lambda i: casemappings[i][2] - (casemappings[i][1] - casemappings[i][0]))
    print_index_table('lowcase_ind', lowcase_ind)


########### main:

if __name__ == "__main__":
    main()
