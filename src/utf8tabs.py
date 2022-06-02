#!python
import pandas as pd
import numpy as np

_UNICODE_DIR = "https://www.unicode.org/Public/14.0.0/ucd"

def read_unidata(category='Lu', casetype='lowcase', big=False):
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


def make_caselist(df):
    letters=[]
    for idx, row in df.iterrows():
        #print(idx+1, ':', row['code'], row['lowcase'] - row['code'], ',', chr(row['code']), chr(row['lowcase']), ',', row['name'])
        letters.append([idx+1, row['code'], row['lowcase'], row['name']])
    return letters


def make_casefold(letters):
    prevoffset = 0
    diffoffset = 0
    prev = [-1, 0, 0]
    diff = [-1, 0, 0]

    out = []
    n = 1
    for x in letters:
        offset = x[2] - x[1]
        diffoffset = prevoffset - offset
        if (diff[1] and x[1] - prev[1] != diff[1]) or (diff[2] and x[2] - prev[2] != diff[2]) or prevoffset != offset:
            out.append([x[1], x[2], n, x[3]]) # , ';', chr(x[1]), chr(x[2]), ';', x[1], offset, "CHANGE")
            #print(x[1], x[2], ';', chr(x[1]), chr(x[2]), ';', offset, "CHANGE", n+1)
            diff[1] = 0
            diff[2] = 0
            n = 1
        else:
            n += 1
            if diff[1] == 0:
                diff[1] = x[1] - prev[1]
                diff[2] = x[2] - prev[2]
            #print(x[1], x[2], ';', chr(x[1]), chr(x[2]), ';', offset)
            diff[1] = x[1] - prev[1]
            diff[2] = x[2] - prev[2]
        prev[2] = x[2]
        prev[1] = x[1]
        prevoffset = offset

    out.append(out[-1])
    out[-1][2] = 26
    cfold = []
    for i in range(0, len(out)-1):
        d = out[i][1] - out[i][0]
        cfold.append([out[i][0], out[i+1][2], d, out[i][3]])
    return cfold


def print_casefold(cfold):
    print('#include <stdint.h>\n')
    print('struct CaseFold { uint16_t c0, c1, m1; };\n')
    print('static struct CaseFold casefold[] = {\n   ', end='')
    n = 1
    s = 5
    count = 0
    table = []
    for x in cfold:
        d = 2 if abs(x[2]) == 1 else 1
        a = x[0]
        b = x[0] + (x[1] - 1)*d
        c = b + x[2]
        if b >= 1<<16 or c >= 1<<16: # only to make sure...
            break
        #print(' {%d, %d, %d}, // %s %s, %s\n   ' % (a, b, c, chr(a), chr(a + x[2]), x[3]), end='')
        if True: # compact
            if n == s:
                n = 0
                if a > 1000:
                    s = 4
                print('\n   ', end='')
            print(' {%d, %d, %d},' % (a, b, c), end='')
            table.append((a, b, c))
        else:
            print(' {%d, %d, %d}, // ' % (a, b, c), end='')
            for y in range(x[0], x[0] + x[1], d):
                print('%s %s, ' % (chr(y), chr(y + x[2])), end='')
            print('')
        count += 1
        n += 1
    print('\n}; // %d\n' % (count))
    return table


def make_casetable():
    df = read_casefold()
    #df = read_unidata()
    letters = make_caselist(df)
    cfold = make_casefold(letters)
    return cfold


def print_casefold_inverse(table):
    cfold_low = [i for i in range(len(table))]
    cfold_low.sort(key=lambda i: table[i][2] - (table[i][1] - table[i][0]))

    print('static uint8_t cfold_low[] = {\n   ', end='')
    for i in range(len(cfold_low)):
        print(" %d," % (cfold_low[i]), end='\n   ' if (i+1) % 20 == 0 else '')
    print('\n};')


########### main:

if __name__ == "__main__":
    cfold = make_casetable()
    table = print_casefold(cfold)
    print_casefold_inverse(table)
