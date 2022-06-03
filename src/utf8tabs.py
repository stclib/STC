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
        caselist.append([idx+1, row['code'], row[casetype], row['name']])
    return caselist


def make_table(caselist):
    prevoffset = 0
    prev = [-1, 0, 0]
    diff = [-1, 0, 0]
    n = 1

    ranges = []
    for x in caselist:
        offset = x[2] - x[1]
        if (diff[1] and x[1] - prev[1] != diff[1]) or (diff[2] and x[2] - prev[2] != diff[2]) or prevoffset != offset:
            ranges.append([x[1], x[2], n, x[3]])
            diff[1] = 0
            diff[2] = 0
            n = 1
        else:
            n += 1
            if diff[1] == 0:
                diff[1] = x[1] - prev[1]
                diff[2] = x[2] - prev[2]
            diff[1] = x[1] - prev[1]
            diff[2] = x[2] - prev[2]
        prev[2] = x[2]
        prev[1] = x[1]
        prevoffset = offset

    ranges.append(ranges[-1])
    ranges[-1][2] = 26

    #for r in ranges:
    #    print("%04X, %04X, %d" % (r[0], r[1], r[2]))
    #exit()

    # next two for loops could be combined to eliminate rangelist
    rangelist = []
    for i in range(0, len(ranges)-1):
        d = ranges[i][1] - ranges[i][0]
        rangelist.append([ranges[i][0], ranges[i+1][2], d, ranges[i][3]])

    table = []
    for x in rangelist:
        if x[2] == -1:
            print("ohoh:", x[0])
        delta = 2 if x[2] == 1 else 1
        a = x[0]
        b = x[0] + (x[1] - 1)*delta
        c = b + x[2]
        if b >= 1<<16 or c >= 1<<16: # only to make sure...
            break
        table.append((a, b, c, x[3]))
    return table


def print_table(name, table, style=1):
    print('static struct CaseFold %s[] = {' % (name))
    for a,b,c,t in table:
        if style == 1:   # first char with name
            print('    {%3d, %3d, %3d},    \t// %s %s, %s' % (a, b, c, chr(a), chr(a + c - b), t))
        elif style == 2: # all chars
            #print('    {%3d, %3d, %3d},    \t// ' % (a, b, c), end='')
            print('    {0x%04X, 0x%04X, 0x%04X},\t// ' % (a, b, c), end='')
            n = 0
            for k in range(a, b+1, 2 if c - b == 1 else 1):
                n += 1
                if n % 17 == 0:
                    print('\n                          \t// ', end='')
                print('%s %s, ' % (chr(k), chr(k + c - b)), end='')
            print('')
    print('}; // %d\n' % (len(table)))


def print_table_inv(name, table):
    lowcase = [i for i in range(len(table))]
    # sort by mapped value table[:][2] (= lowcase) of the first element in each range entry
    lowcase.sort(key=lambda i: table[i][2] - (table[i][1] - table[i][0]))

    print('static uint8_t %s[%d] = {\n   ' % (name, len(table)), end='')
    for i in range(len(lowcase)):
        print(" %d," % (lowcase[i]), end='\n   ' if (i+1) % 20 == 0 else '')
    print('\n};')


def compile_table(name, casetype='lowcase', category=None):
    if category:
        df = read_unidata(casetype, category)
    else:
        df = read_casefold()
    caselist = make_caselist(df, casetype)
    table = make_table(caselist)
    print_table(name, table, 2)
    return table


def main():
    print('#include <stdint.h>\n')
    print('struct CaseFold { uint16_t c0, c1, m1; };\n')

    table = compile_table('casefold') # casefold
    #table = compile_table('tolower_tab', 'lowcase', 'Lu') # unicode
    print_table_inv('cfold_low', table)


########### main:

if __name__ == "__main__":
    main()
