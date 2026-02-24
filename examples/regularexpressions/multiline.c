#include <stc/cregex.h>
#include <stc/csview.h>
#include <stdio.h>

const char* hay = "\
rabbit         54 true\n\
groundhog    2 true\n\
does not match\n\
fox   109    false\n\
";

typedef struct {csview anim; int id; bool ok; } Field;

static inline bool Field_eq(const Field* a, const Field* b)
	{ return csview_eq(&a->anim, &b->anim) && a->id == b->id && a->ok == b->ok; }

#define T Fields, Field
#define i_eq Field_eq
#include <stc/stack.h>

int main(void) {
	cregex re = cregex_from("(?m)^\\s*(\\S+)\\s+([0-9]+)\\s+(true|false)\\s*$");
	c_assert(!re.error);
	Fields fields = {0};

	for (c_match(c, &re, hay)) {
		Fields_push(&fields, (Field){c.match[1], atoi(c.match[2].buf), c.match[3].buf[0] == 't'});
	}

	Fields answer = c_make(Fields, {
	    {c_sv("rabbit"), 54, true},
	    {c_sv("groundhog"), 2, true},
	    {c_sv("fox"), 109, false},
	});
	c_assert(Fields_eq(&fields, &answer));
	
	for (c_each_ref(e, Fields, fields))
		printf(c_svfmt ", %d, %s\n", c_svarg(e->anim), e->id, e->ok?"true":"false");

	c_drop(Fields, &fields, &answer);
}

/*
// https://shadow.github.io/docs/rust/regex/struct.Regex.html#example-extracting-capture-groups
use regex::Regex;

fn main() {
    let hay = "
    rabbit         54 true
    groundhog 2 true
    does not match
    fox   109    false
    ";
    
    let re = Regex::new(r"(?m)^\s*(\S+)\s+([0-9]+)\s+(true|false)\s*$").unwrap();
    let mut fields: Vec<(&str, i64, bool)> = vec![];
    
    for (_, [f1, f2, f3]) in re.captures_iter(hay).map(|caps| caps.extract()) {
        fields.push((f1, f2.parse()?, f3.parse()?));
    }
    
    assert_eq!(fields, vec![
        ("rabbit", 54, true),
        ("groundhog", 2, true),
        ("fox", 109, false),
    ]);
}
*/