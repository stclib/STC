#define JSMN_PARENT_LINKS
#define JSMN_STRICT
#include "stc/sys/jsmn.h"
#include "stc/c11/serialize.h"

static inline csview json_tok(const char* js, jsmntok_t tok) {
    return c_sv_2(js + tok.start, (isize_t)(tok.end - tok.start));
}

jsmntok_t* json_tokenize(const char* js, size_t js_len, int* ret) {
    unsigned int cap = 128;
    jsmntok_t* tokens = NULL;

    jsmn_parser parser;
    jsmn_init(&parser);
    do {
        cap += cap/2;
        tokens = realloc(tokens, sizeof(jsmntok_t)*cap);
        *ret = jsmn_parse(&parser, js, js_len, tokens, cap);
    } while (*ret == JSMN_ERROR_NOMEM);

    switch (*ret) {
        case JSMN_ERROR_INVAL:
        case JSMN_ERROR_PART: break;
        default: return tokens;
    }
    free(tokens);
    return NULL;
}


// Recursive function to visit nodes
// - tokens : the parsed token array
// - pos    : current token position in the array
// - depth  : current nesting depth
void visit_nodes(csviewz js, jsmntok_t *tokens, int* pos, int depth, bool iskey) {
    jsmntok_t *tok = &tokens[*pos];

    if (tok->type == JSMN_OBJECT) {
        // Object tokens contain key-value pairs. The first child is the first key
        printf("#%d {\n", *pos);
        for (int child = 0; child < tok->size; ++child) {
            *pos += 1; // visit key (iskey=true)
            visit_nodes(js, tokens, pos, depth + 1, true);
            *pos += 1; // visit val
            visit_nodes(js, tokens, pos, depth + 1, false);
        }
        printf("%*s}\n", 2*depth, "");
    }
    else if (tok->type == JSMN_ARRAY) {
        // Array tokens contains elements
        printf("[\n");
        for (int child = 0; child < tok->size; child++) {
            printf("%*s", 2*(depth + 1), "");
            *pos += 1; // visit next element
            visit_nodes(js, tokens, pos, depth + 1, false);
        }
        printf("%*s]\n", 2*depth, "");
    }
    else if (tok->type == JSMN_STRING) {
        if (iskey)
            printf("%*s%.*s: ", 2*depth, "", c_svarg(json_tok(js.str, tokens[*pos])));
        else {
            cstr _tmp = {0};
            printf("#%d ", *pos);
            printf("\"%s\"\n", cjson_unescape_str(json_tok(js.str, tokens[*pos]), &_tmp).str);
            cstr_drop(&_tmp);
        }
    }
    else if (tok->type == JSMN_PRIMITIVE) {
        printf("#%d ", *pos);
        printf("%.*s\n", c_svarg(json_tok(js.str, tokens[*pos])));
    }
}

int traverse_jsmn(csviewz json) {
    int token_count, pos = 0;
    jsmntok_t* tokens = json_tokenize(json.str, (size_t)json.size, &token_count);
    if (tokens) {
        // Start recursion from the root token (index 0)
        visit_nodes(json, tokens, &pos, 0, false);
    }
    free(tokens);
    return token_count;
}

const char* json_escape_str(const char *in, isize_t in_len, cstr* out);
const char* json_unescape_str(const char *in, isize_t in_len, cstr* out);


int main(void) // (int argc, char* argv[])
{
    cstr json_str = {0};
    json_str = cstr_from_file("SoundbanksInfo.json");
    //cstr_str json = cstr_from("{\"user\": \"johndoe\", \"uid\": 1000}");
    //json_str = cstr_from_file("serde_test.json");
    //json_str = cstr_from_file("departments.json");
    csviewz json = cstr_zv(&json_str);
    traverse_jsmn(json);
    return 0;
}
