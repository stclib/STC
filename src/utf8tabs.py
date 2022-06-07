#!python
import pandas as pd
import numpy as np

_UNICODE_DIR = "https://www.unicode.org/Public/14.0.0/ucd"


def read_unidata(casetype='lowcase', category='Lu', big=False):
    df = pd.read_csv(_UNICODE_DIR+'/UnicodeData.txt', sep=';', converters={0: lambda x: int(x, base=16)},
                      names=['code', 'name', 'category', 'canclass', 'bidircat', 'chrdecomp',
                             'decdig', 'digval', 'numval', 'mirrored', 'uc1name', 'comment',
                             'upcase', 'lowcase', 'titlecase'],
                      usecols=['code', 'name', 'category', 'bidircat', 'upcase', 'lowcase', 'titlecase'])
    if big:
        df = df[df['code'] >= (1<<16)]
    else:
        df = df[df['code'] < (1<<16)]
    
    if category:
        df = df[df['category'] == category]
    df = df.replace(np.nan, '0')
    for k in ['upcase', 'lowcase', 'titlecase']:
        df[k] = df[k].apply(int, base=16)

    if casetype:                   # 'lowcase', 'upcase', 'titlecase'
        df = df[df[casetype] != 0] # remove mappings to 0
    return df


def read_casefold(big=False):
    df = pd.read_csv(_UNICODE_DIR+'/CaseFolding.txt', engine='python', sep='; #? ?', comment='#',
                     converters={0: lambda x: int(x, base=16)},
                     names=['code', 'status', 'lowcase', 'name']) # comment => 'name'
    if big:
        df = df[df['code'] >= (1<<16)]
    else:
        df = df[df['code'] < (1<<16)]

    df = df[df.status.isin(['S', 'C'])]
    df['lowcase'] = df['lowcase'].apply(int, base=16)
    return df


def make_caselist(df, casetype):
    caselist=[]
    for idx, row in df.iterrows():
        caselist.append([row['code'], row[casetype], row['name']])
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
            if  j > 0 and start_a not in [0xAB70, 0x13F8]: # BUG in CaseFolding.txt V14
                table.append([start_a, prev_a, prev_b, start_name])
            if j < n_1:
                diff_a = caselist[j+1][0] - a
                diff_b = caselist[j+1][1] - b
                start_a = a
                start_name = name

        prev_a, prev_b = a, b
        prev_offs = offset

    table.append([start_a, a, b, start_name])
    return table


def print_table(name, table, style=1):
    print('static struct CaseMapping %s[] = {' % (name))
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


def make_inverse_ind(table):
    inv = []
    for i in range(len(table)):
        if table[i][2] not in [ord('i'), ord('s')]: # ignore 'i'. 's' upcase mappings.
            inv.append(i)
    # sort by mapped value table[:][2] (= inv) of the first element in each range entry
    inv.sort(key=lambda i: table[i][2] - (table[i][1] - table[i][0]))
    return inv


def compile_table(casetype='lowcase', category=None):
    if category:
        df = read_unidata(casetype, category)
    else:
        df = read_casefold()
    caselist = make_caselist(df, casetype)
    table = make_table(caselist)
    return table


def main():
    print('#include <stdint.h>\n')
    print('struct CaseMapping { uint16_t c0, c1, m1; };\n')

    casemappings = compile_table('lowcase') # Casefolding.txt
    upcase       = compile_table('lowcase', 'Lu') # UnicodeData.txt uppercase

    # merge in additional Lu => Ll mappings from UnicodeData.txt
    for v in upcase:
        if v not in casemappings:
            casemappings.append(v)

    # sort casemappings by uppercase values:
    casemappings.sort(key=lambda x: x[0])
    print_table('casemappings', casemappings, style=1)
    
    # index list sorted by mapped lowercase values:
    upcase_ind = make_inverse_ind(casemappings)
    print('enum { casefold_len = %d };' % len(casemappings))
    print_index_table('upcase_ind', upcase_ind)


########### main:

if __name__ == "__main__":
    main()
