// https://www.janekbieser.dev/posts/exploring-tagged-unions/
// https://godbolt.org/z/Pd815sxfb // compare Rust - STC
#include <stdio.h>
#include <stc/algorithm.h>

c_union (Event,
    (Event_KeyPress, int),
    (Event_MouseClick, struct {int x, y;}),
    (Event_MouseMove, struct {int x, y;}),
);

int process_event(Event evt) {
    c_when (&evt) {
        c_is(Event_KeyPress, key_code) return *key_code * 1234;
        c_is(Event_MouseClick, pos) return pos->x + pos->y * 457;
        c_is(Event_MouseMove, delta) return delta->x + delta->y * 987;
    }
    return 0;
}

int main(void) {
    for (c_items(evt, Event, {
        c_variant(Event_KeyPress, 100),
        c_variant(Event_MouseClick, {10, 20}),
        c_variant(Event_MouseMove, {3, 1}),
    })){
        int result = process_event(*evt.ref);
        printf("Event Result: %d\n", result);
    }
}
