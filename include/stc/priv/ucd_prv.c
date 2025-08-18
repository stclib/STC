/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef STC_UCD_PRV_C_INCLUDED
#define STC_UCD_PRV_C_INCLUDED

#include <ctype.h>

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.o.

enum {
    U8G_Cc, U8G_Lt, U8G_Nd, U8G_Nl,
    U8G_Pc, U8G_Pd, U8G_Pe, U8G_Pf, U8G_Pi, U8G_Po, U8G_Ps,
    U8G_Sc, U8G_Zl, U8G_Zp, U8G_Zs,
    U8G_Arabic, U8G_Bengali, U8G_Cyrillic,
    U8G_Devanagari, U8G_Georgian, U8G_Greek,
    U8G_Han, U8G_Hiragana, U8G_Katakana,
    U8G_Latin, U8G_Thai,
    U8G_SIZE
};

static bool utf8_isgroup(int group, uint32_t c);

static bool utf8_isalpha(uint32_t c) {
    static int16_t groups[] = {U8G_Latin, U8G_Nl, U8G_Cyrillic, U8G_Han, U8G_Devanagari,
                               U8G_Arabic, U8G_Bengali, U8G_Hiragana, U8G_Katakana,
                               U8G_Thai, U8G_Greek, U8G_Georgian};
    if (c < 128) return isalpha((int)c) != 0;
    for (int j=0; j < (int)(sizeof groups/sizeof groups[0]); ++j)
        if (utf8_isgroup(groups[j], c))
            return true;
    return false;
}

static bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha((int)c) != 0;
    return utf8_islower(c) || utf8_isupper(c) ||
           utf8_isgroup(U8G_Lt, c);
}

static bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum((int)c) != 0;
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c);
}

static bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum((int)c) != 0) | (c == '_');
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c) ||
           utf8_isgroup(U8G_Pc, c);
}

static bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

static bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace((int)c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

/* The tables below are extracted from the RE2 library */
typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

static const URange16 Cc_range16[] = { // Control
    { 0, 31 },
    { 127, 159 },
};

static const URange16 Lt_range16[] = { // Title case
    { 453, 453 },
    { 456, 456 },
    { 459, 459 },
    { 498, 498 },
    { 8072, 8079 },
    { 8088, 8095 },
    { 8104, 8111 },
    { 8124, 8124 },
    { 8140, 8140 },
    { 8188, 8188 },
};

static const URange16 Nd_range16[] = { // Decimal number
    { 48, 57 },
    { 1632, 1641 },
    { 1776, 1785 },
    { 1984, 1993 },
    { 2406, 2415 },
    { 2534, 2543 },
    { 2662, 2671 },
    { 2790, 2799 },
    { 2918, 2927 },
    { 3046, 3055 },
    { 3174, 3183 },
    { 3302, 3311 },
    { 3430, 3439 },
    { 3558, 3567 },
    { 3664, 3673 },
    { 3792, 3801 },
    { 3872, 3881 },
    { 4160, 4169 },
    { 4240, 4249 },
    { 6112, 6121 },
    { 6160, 6169 },
    { 6470, 6479 },
    { 6608, 6617 },
    { 6784, 6793 },
    { 6800, 6809 },
    { 6992, 7001 },
    { 7088, 7097 },
    { 7232, 7241 },
    { 7248, 7257 },
    { 42528, 42537 },
    { 43216, 43225 },
    { 43264, 43273 },
    { 43472, 43481 },
    { 43504, 43513 },
    { 43600, 43609 },
    { 44016, 44025 },
    { 65296, 65305 },
};

static const URange16 Nl_range16[] = { // Number letter
    { 5870, 5872 },
    { 8544, 8578 },
    { 8581, 8584 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12346 },
    { 42726, 42735 },
};

static const URange16 Pc_range16[] = { // Connector punctuation
    { 95, 95 },
    { 8255, 8256 },
    { 8276, 8276 },
    { 65075, 65076 },
    { 65101, 65103 },
    { 65343, 65343 },
};

static const URange16 Pd_range16[] = { // Dash punctuation
    { 45, 45 },
    { 1418, 1418 },
    { 1470, 1470 },
    { 5120, 5120 },
    { 6150, 6150 },
    { 8208, 8213 },
    { 11799, 11799 },
    { 11802, 11802 },
    { 11834, 11835 },
    { 11840, 11840 },
    { 11869, 11869 },
    { 12316, 12316 },
    { 12336, 12336 },
    { 12448, 12448 },
    { 65073, 65074 },
    { 65112, 65112 },
    { 65123, 65123 },
    { 65293, 65293 },
};

static const URange16 Pe_range16[] = {
    { 41, 41 },
    { 93, 93 },
    { 125, 125 },
    { 3899, 3899 },
    { 3901, 3901 },
    { 5788, 5788 },
    { 8262, 8262 },
    { 8318, 8318 },
    { 8334, 8334 },
    { 8969, 8969 },
    { 8971, 8971 },
    { 9002, 9002 },
    { 10089, 10089 },
    { 10091, 10091 },
    { 10093, 10093 },
    { 10095, 10095 },
    { 10097, 10097 },
    { 10099, 10099 },
    { 10101, 10101 },
    { 10182, 10182 },
    { 10215, 10215 },
    { 10217, 10217 },
    { 10219, 10219 },
    { 10221, 10221 },
    { 10223, 10223 },
    { 10628, 10628 },
    { 10630, 10630 },
    { 10632, 10632 },
    { 10634, 10634 },
    { 10636, 10636 },
    { 10638, 10638 },
    { 10640, 10640 },
    { 10642, 10642 },
    { 10644, 10644 },
    { 10646, 10646 },
    { 10648, 10648 },
    { 10713, 10713 },
    { 10715, 10715 },
    { 10749, 10749 },
    { 11811, 11811 },
    { 11813, 11813 },
    { 11815, 11815 },
    { 11817, 11817 },
    { 11862, 11862 },
    { 11864, 11864 },
    { 11866, 11866 },
    { 11868, 11868 },
    { 12297, 12297 },
    { 12299, 12299 },
    { 12301, 12301 },
    { 12303, 12303 },
    { 12305, 12305 },
    { 12309, 12309 },
    { 12311, 12311 },
    { 12313, 12313 },
    { 12315, 12315 },
    { 12318, 12319 },
    { 64830, 64830 },
    { 65048, 65048 },
    { 65078, 65078 },
    { 65080, 65080 },
    { 65082, 65082 },
    { 65084, 65084 },
    { 65086, 65086 },
    { 65088, 65088 },
    { 65090, 65090 },
    { 65092, 65092 },
    { 65096, 65096 },
    { 65114, 65114 },
    { 65116, 65116 },
    { 65118, 65118 },
    { 65289, 65289 },
    { 65341, 65341 },
    { 65373, 65373 },
    { 65376, 65376 },
    { 65379, 65379 },
};

static const URange16 Pf_range16[] = { // Final punctuation
    { 187, 187 },
    { 8217, 8217 },
    { 8221, 8221 },
    { 8250, 8250 },
    { 11779, 11779 },
    { 11781, 11781 },
    { 11786, 11786 },
    { 11789, 11789 },
    { 11805, 11805 },
    { 11809, 11809 },
};

static const URange16 Pi_range16[] = { // Initial punctuation
    { 171, 171 },
    { 8216, 8216 },
    { 8219, 8220 },
    { 8223, 8223 },
    { 8249, 8249 },
    { 11778, 11778 },
    { 11780, 11780 },
    { 11785, 11785 },
    { 11788, 11788 },
    { 11804, 11804 },
    { 11808, 11808 },
};

static const URange16 Po_range16[] = {
    { 33, 35 },
    { 37, 39 },
    { 42, 42 },
    { 44, 44 },
    { 46, 47 },
    { 58, 59 },
    { 63, 64 },
    { 92, 92 },
    { 161, 161 },
    { 167, 167 },
    { 182, 183 },
    { 191, 191 },
    { 894, 894 },
    { 903, 903 },
    { 1370, 1375 },
    { 1417, 1417 },
    { 1472, 1472 },
    { 1475, 1475 },
    { 1478, 1478 },
    { 1523, 1524 },
    { 1545, 1546 },
    { 1548, 1549 },
    { 1563, 1563 },
    { 1565, 1567 },
    { 1642, 1645 },
    { 1748, 1748 },
    { 1792, 1805 },
    { 2039, 2041 },
    { 2096, 2110 },
    { 2142, 2142 },
    { 2404, 2405 },
    { 2416, 2416 },
    { 2557, 2557 },
    { 2678, 2678 },
    { 2800, 2800 },
    { 3191, 3191 },
    { 3204, 3204 },
    { 3572, 3572 },
    { 3663, 3663 },
    { 3674, 3675 },
    { 3844, 3858 },
    { 3860, 3860 },
    { 3973, 3973 },
    { 4048, 4052 },
    { 4057, 4058 },
    { 4170, 4175 },
    { 4347, 4347 },
    { 4960, 4968 },
    { 5742, 5742 },
    { 5867, 5869 },
    { 5941, 5942 },
    { 6100, 6102 },
    { 6104, 6106 },
    { 6144, 6149 },
    { 6151, 6154 },
    { 6468, 6469 },
    { 6686, 6687 },
    { 6816, 6822 },
    { 6824, 6829 },
    { 7002, 7008 },
    { 7037, 7038 },
    { 7164, 7167 },
    { 7227, 7231 },
    { 7294, 7295 },
    { 7360, 7367 },
    { 7379, 7379 },
    { 8214, 8215 },
    { 8224, 8231 },
    { 8240, 8248 },
    { 8251, 8254 },
    { 8257, 8259 },
    { 8263, 8273 },
    { 8275, 8275 },
    { 8277, 8286 },
    { 11513, 11516 },
    { 11518, 11519 },
    { 11632, 11632 },
    { 11776, 11777 },
    { 11782, 11784 },
    { 11787, 11787 },
    { 11790, 11798 },
    { 11800, 11801 },
    { 11803, 11803 },
    { 11806, 11807 },
    { 11818, 11822 },
    { 11824, 11833 },
    { 11836, 11839 },
    { 11841, 11841 },
    { 11843, 11855 },
    { 11858, 11860 },
    { 12289, 12291 },
    { 12349, 12349 },
    { 12539, 12539 },
    { 42238, 42239 },
    { 42509, 42511 },
    { 42611, 42611 },
    { 42622, 42622 },
    { 42738, 42743 },
    { 43124, 43127 },
    { 43214, 43215 },
    { 43256, 43258 },
    { 43260, 43260 },
    { 43310, 43311 },
    { 43359, 43359 },
    { 43457, 43469 },
    { 43486, 43487 },
    { 43612, 43615 },
    { 43742, 43743 },
    { 43760, 43761 },
    { 44011, 44011 },
    { 65040, 65046 },
    { 65049, 65049 },
    { 65072, 65072 },
    { 65093, 65094 },
    { 65097, 65100 },
    { 65104, 65106 },
    { 65108, 65111 },
    { 65119, 65121 },
    { 65128, 65128 },
    { 65130, 65131 },
    { 65281, 65283 },
    { 65285, 65287 },
    { 65290, 65290 },
    { 65292, 65292 },
    { 65294, 65295 },
    { 65306, 65307 },
    { 65311, 65312 },
    { 65340, 65340 },
    { 65377, 65377 },
    { 65380, 65381 },
};

static const URange16 Ps_range16[] = {
    { 40, 40 },
    { 91, 91 },
    { 123, 123 },
    { 3898, 3898 },
    { 3900, 3900 },
    { 5787, 5787 },
    { 8218, 8218 },
    { 8222, 8222 },
    { 8261, 8261 },
    { 8317, 8317 },
    { 8333, 8333 },
    { 8968, 8968 },
    { 8970, 8970 },
    { 9001, 9001 },
    { 10088, 10088 },
    { 10090, 10090 },
    { 10092, 10092 },
    { 10094, 10094 },
    { 10096, 10096 },
    { 10098, 10098 },
    { 10100, 10100 },
    { 10181, 10181 },
    { 10214, 10214 },
    { 10216, 10216 },
    { 10218, 10218 },
    { 10220, 10220 },
    { 10222, 10222 },
    { 10627, 10627 },
    { 10629, 10629 },
    { 10631, 10631 },
    { 10633, 10633 },
    { 10635, 10635 },
    { 10637, 10637 },
    { 10639, 10639 },
    { 10641, 10641 },
    { 10643, 10643 },
    { 10645, 10645 },
    { 10647, 10647 },
    { 10712, 10712 },
    { 10714, 10714 },
    { 10748, 10748 },
    { 11810, 11810 },
    { 11812, 11812 },
    { 11814, 11814 },
    { 11816, 11816 },
    { 11842, 11842 },
    { 11861, 11861 },
    { 11863, 11863 },
    { 11865, 11865 },
    { 11867, 11867 },
    { 12296, 12296 },
    { 12298, 12298 },
    { 12300, 12300 },
    { 12302, 12302 },
    { 12304, 12304 },
    { 12308, 12308 },
    { 12310, 12310 },
    { 12312, 12312 },
    { 12314, 12314 },
    { 12317, 12317 },
    { 64831, 64831 },
    { 65047, 65047 },
    { 65077, 65077 },
    { 65079, 65079 },
    { 65081, 65081 },
    { 65083, 65083 },
    { 65085, 65085 },
    { 65087, 65087 },
    { 65089, 65089 },
    { 65091, 65091 },
    { 65095, 65095 },
    { 65113, 65113 },
    { 65115, 65115 },
    { 65117, 65117 },
    { 65288, 65288 },
    { 65339, 65339 },
    { 65371, 65371 },
    { 65375, 65375 },
    { 65378, 65378 },
};

static const URange16 Sc_range16[] = { // Currency symbol
    { 36, 36 },
    { 162, 165 },
    { 1423, 1423 },
    { 1547, 1547 },
    { 2046, 2047 },
    { 2546, 2547 },
    { 2555, 2555 },
    { 2801, 2801 },
    { 3065, 3065 },
    { 3647, 3647 },
    { 6107, 6107 },
    { 8352, 8384 },
    { 43064, 43064 },
    { 65020, 65020 },
    { 65129, 65129 },
    { 65284, 65284 },
    { 65504, 65505 },
    { 65509, 65510 },
};

static const URange16 Zl_range16[] = { // Line separator
    { 8232, 8232 },
};

static const URange16 Zp_range16[] = { // Paragraph separator
    { 8233, 8233 },
};

static const URange16 Zs_range16[] = { // Space separator
    { 32, 32 },
    { 160, 160 },
    { 5760, 5760 },
    { 8192, 8202 },
    { 8239, 8239 },
    { 8287, 8287 },
    { 12288, 12288 },
};

static const URange16 Arabic_range16[] = {
    { 1536, 1540 },
    { 1542, 1547 },
    { 1549, 1562 },
    { 1564, 1566 },
    { 1568, 1599 },
    { 1601, 1610 },
    { 1622, 1647 },
    { 1649, 1756 },
    { 1758, 1791 },
    { 1872, 1919 },
    { 2160, 2190 },
    { 2192, 2193 },
    { 2200, 2273 },
    { 2275, 2303 },
    { 64336, 64450 },
    { 64467, 64829 },
    { 64832, 64911 },
    { 64914, 64967 },
    { 64975, 64975 },
    { 65008, 65023 },
    { 65136, 65140 },
    { 65142, 65276 },
};

static const URange16 Bengali_range16[] = {
    { 2432, 2435 },
    { 2437, 2444 },
    { 2447, 2448 },
    { 2451, 2472 },
    { 2474, 2480 },
    { 2482, 2482 },
    { 2486, 2489 },
    { 2492, 2500 },
    { 2503, 2504 },
    { 2507, 2510 },
    { 2519, 2519 },
    { 2524, 2525 },
    { 2527, 2531 },
    { 2534, 2558 },
};

static const URange16 Cyrillic_range16[] = {
    { 1024, 1156 },
    { 1159, 1327 },
    { 7296, 7304 },
    { 7467, 7467 },
    { 7544, 7544 },
    { 11744, 11775 },
    { 42560, 42655 },
    { 65070, 65071 },
};

static const URange16 Devanagari_range16[] = {
    { 2304, 2384 },
    { 2389, 2403 },
    { 2406, 2431 },
    { 43232, 43263 },
};

static const URange16 Georgian_range16[] = {
    { 4256, 4293 },
    { 4295, 4295 },
    { 4301, 4301 },
    { 4304, 4346 },
    { 4348, 4351 },
    { 7312, 7354 },
    { 7357, 7359 },
    { 11520, 11557 },
    { 11559, 11559 },
    { 11565, 11565 },
};

static const URange16 Greek_range16[] = {
    { 880, 883 },
    { 885, 887 },
    { 890, 893 },
    { 895, 895 },
    { 900, 900 },
    { 902, 902 },
    { 904, 906 },
    { 908, 908 },
    { 910, 929 },
    { 931, 993 },
    { 1008, 1023 },
    { 7462, 7466 },
    { 7517, 7521 },
    { 7526, 7530 },
    { 7615, 7615 },
    { 7936, 7957 },
    { 7960, 7965 },
    { 7968, 8005 },
    { 8008, 8013 },
    { 8016, 8023 },
    { 8025, 8025 },
    { 8027, 8027 },
    { 8029, 8029 },
    { 8031, 8061 },
    { 8064, 8116 },
    { 8118, 8132 },
    { 8134, 8147 },
    { 8150, 8155 },
    { 8157, 8175 },
    { 8178, 8180 },
    { 8182, 8190 },
    { 8486, 8486 },
    { 43877, 43877 },
};

static const URange16 Han_range16[] = {
    { 11904, 11929 },
    { 11931, 12019 },
    { 12032, 12245 },
    { 12293, 12293 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12347 },
    { 13312, 19903 },
    { 19968, 40959 },
    { 63744, 64109 },
    { 64112, 64217 },
};

static const URange16 Hiragana_range16[] = {
    { 12353, 12438 },
    { 12445, 12447 },
};

static const URange16 Katakana_range16[] = {
    { 12449, 12538 },
    { 12541, 12543 },
    { 12784, 12799 },
    { 13008, 13054 },
    { 13056, 13143 },
    { 65382, 65391 },
    { 65393, 65437 },
};

static const URange16 Latin_range16[] = {
    { 65, 90 },
    { 97, 122 },
    { 170, 170 },
    { 186, 186 },
    { 192, 214 },
    { 216, 246 },
    { 248, 696 },
    { 736, 740 },
    { 7424, 7461 },
    { 7468, 7516 },
    { 7522, 7525 },
    { 7531, 7543 },
    { 7545, 7614 },
    { 7680, 7935 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 8490, 8491 },
    { 8498, 8498 },
    { 8526, 8526 },
    { 8544, 8584 },
    { 11360, 11391 },
    { 42786, 42887 },
    { 42891, 42954 },
    { 42960, 42961 },
    { 42963, 42963 },
    { 42965, 42969 },
    { 42994, 43007 },
    { 43824, 43866 },
    { 43868, 43876 },
    { 43878, 43881 },
    { 64256, 64262 },
    { 65313, 65338 },
    { 65345, 65370 },
};

static const URange16 Thai_range16[] = {
    { 3585, 3642 },
    { 3648, 3675 },
};

#ifdef __cplusplus
    #define _e_arg(k, v) v
#else
    #define _e_arg(k, v) [k] = v
#endif
#define UNI_ENTRY(Code) { Code##_range16, sizeof(Code##_range16)/sizeof(URange16) }

typedef struct {
  const URange16 *r16;
  int nr16;
} UGroup;

static const UGroup _utf8_unicode_groups[U8G_SIZE] = {
    _e_arg(U8G_Cc, UNI_ENTRY(Cc)),
    _e_arg(U8G_Lt, UNI_ENTRY(Lt)),
    _e_arg(U8G_Nd, UNI_ENTRY(Nd)),
    _e_arg(U8G_Nl, UNI_ENTRY(Nl)),
    _e_arg(U8G_Pc, UNI_ENTRY(Pc)),
    _e_arg(U8G_Pd, UNI_ENTRY(Pd)),
    _e_arg(U8G_Pe, UNI_ENTRY(Pe)),
    _e_arg(U8G_Pf, UNI_ENTRY(Pf)),
    _e_arg(U8G_Pi, UNI_ENTRY(Pi)),
    _e_arg(U8G_Po, UNI_ENTRY(Po)),
    _e_arg(U8G_Ps, UNI_ENTRY(Ps)),
    _e_arg(U8G_Sc, UNI_ENTRY(Sc)),
    _e_arg(U8G_Zl, UNI_ENTRY(Zl)),
    _e_arg(U8G_Zp, UNI_ENTRY(Zp)),
    _e_arg(U8G_Zs, UNI_ENTRY(Zs)),
    _e_arg(U8G_Arabic, UNI_ENTRY(Arabic)),
    _e_arg(U8G_Bengali, UNI_ENTRY(Bengali)),
    _e_arg(U8G_Cyrillic, UNI_ENTRY(Cyrillic)),
    _e_arg(U8G_Devanagari, UNI_ENTRY(Devanagari)),
    _e_arg(U8G_Georgian, UNI_ENTRY(Georgian)),
    _e_arg(U8G_Greek, UNI_ENTRY(Greek)),
    _e_arg(U8G_Han, UNI_ENTRY(Han)),
    _e_arg(U8G_Hiragana, UNI_ENTRY(Hiragana)),
    _e_arg(U8G_Katakana, UNI_ENTRY(Katakana)),
    _e_arg(U8G_Latin, UNI_ENTRY(Latin)),
    _e_arg(U8G_Thai, UNI_ENTRY(Thai)),
};

static bool utf8_isgroup(int group, uint32_t c) {
    for (int j=0; j<_utf8_unicode_groups[group].nr16; ++j) {
        if (c < _utf8_unicode_groups[group].r16[j].lo)
            return false;
        if (c <= _utf8_unicode_groups[group].r16[j].hi)
            return true;
    }
    return false;
}

#endif // STC_UCD_PRV_C_INCLUDED
