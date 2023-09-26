/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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

STC_DEF _cx_iter _cx_MEMB(_advance)(_cx_iter it, intptr_t n) {
    intptr_t len = _cx_MEMB(_size)(it._s);
    intptr_t pos = it.pos, idx = _cdeq_toidx(it._s, pos);
    it.pos = (pos + n) & it._s->capmask; 
    it.ref += it.pos - pos;
    if (!c_less_unsigned(idx + n, len)) it.ref = NULL;
    return it;
}

STC_DEF void
_cx_MEMB(_clear)(_cx_Self* self) {
    c_foreach (i, _cx_Self, *self)
        { i_keydrop(i.ref); }
    self->start = 0, self->end = 0;
}

STC_DEF void
_cx_MEMB(_drop)(_cx_Self* self) {
    _cx_MEMB(_clear)(self);
    i_free(self->cbuf);
}

STC_DEF _cx_Self
_cx_MEMB(_with_capacity)(const intptr_t n) {
    _cx_Self cx = {0};
    _cx_MEMB(_reserve)(&cx, n);
    return cx;
}

STC_DEF bool
_cx_MEMB(_reserve)(_cx_Self* self, const intptr_t n) {
    if (n <= self->capmask)
        return true;
    intptr_t oldcap = self->capmask + 1, newcap = stc_nextpow2(n + 1);
    _cx_value* d = (_cx_value *)i_realloc(self->cbuf, newcap*c_sizeof *self->cbuf);
    if (!d)
        return false;
    intptr_t head = oldcap - self->start;
    if (self->start <= self->end)
        ;
    else if (head < self->end) {
        self->start = newcap - head;
        c_memmove(d + self->start, d + oldcap - head, head*c_sizeof *d);
    } else {
        c_memmove(d + oldcap, d, self->end*c_sizeof *d);
        self->end += oldcap;
    }
    self->capmask = newcap - 1;
    self->cbuf = d;
    return true;
}

STC_DEF _cx_value*
_cx_MEMB(_push)(_cx_Self* self, i_key value) { // push_back
    intptr_t end = (self->end + 1) & self->capmask;
    if (end == self->start) { // full
        _cx_MEMB(_reserve)(self, self->capmask + 3); // => 2x expand
        end = (self->end + 1) & self->capmask;
    }
    _cx_value *v = self->cbuf + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_cx_MEMB(_shrink_to_fit)(_cx_Self *self) {
    intptr_t sz = _cx_MEMB(_size)(self), j = 0;
    if (sz > self->capmask/2)
        return;
    _cx_Self out = _cx_MEMB(_with_capacity)(sz);
    if (!out.cbuf)
        return;
    c_foreach (i, _cx_Self, *self)
        out.cbuf[j++] = *i.ref;
    out.end = sz;
    i_free(self->cbuf);
    *self = out;
}

#if !defined i_no_clone
STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self cx) {
    intptr_t sz = _cx_MEMB(_size)(&cx), j = 0;
    _cx_Self out = _cx_MEMB(_with_capacity)(sz);
    if (out.cbuf)
        c_foreach (i, _cx_Self, cx)
            out.cbuf[j++] = i_keyclone((*i.ref));
    out.end = sz;
    return out;
}
#endif // i_no_clone

#if defined _i_has_eq || defined _i_has_cmp
STC_DEF bool
_cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    if (_cx_MEMB(_size)(self) != _cx_MEMB(_size)(other)) return false;
    for (_cx_iter i = _cx_MEMB(_begin)(self), j = _cx_MEMB(_begin)(other);
         i.ref; _cx_MEMB(_next)(&i), _cx_MEMB(_next)(&j))
    {
        const _cx_raw _rx = i_keyto(i.ref), _ry = i_keyto(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
