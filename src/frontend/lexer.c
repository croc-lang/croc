#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "../string_utils.h"

lexer_t* new_lexer(char* path) {
    lexer_t* lexer = calloc(sizeof(lexer_t), 1);
    string_t* src = NULL;
    size_t len = 0;

    FILE* file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);
    src = sized_string(len);

    if (src == NULL)
        return NULL;

    fread(src->data, len, 1, file);

    lexer->filename = path;
    lexer->src = src;
    lexer->i = 0;

    fclose(file);

    return lexer;
}

static token_t lexing_ident(lexer_t* lexer, size_t start) {
    while (isalnum(string_get(lexer->src, lexer->i++)));

    return init_token(TK_IDENT, string_slice(lexer->src, start, lexer->i - 1));
}

static token_t lexing_number(lexer_t* lexer, size_t start) {
    while (isdigit(string_get(lexer->src, lexer->i++)));

    if (string_get(lexer->src, lexer->i) == '.' &&
        isdigit(string_get(lexer->src, lexer->i + 1))) {
        while (isdigit(string_get(lexer->src, lexer->i++)));

        return init_token(TK_FLOAT,
            string_slice(lexer->src, start, lexer->i - 1));
    }

    return init_token(TK_INT, string_slice(lexer->src, start, lexer->i - 1));
}

static token_t lexing_string(lexer_t* lexer) {
    string_t* str = new_string("");
    char c;

    while ((c = string_get(lexer->src, lexer->i++)) != -1) {
        if (c == '\\') {
            switch ((c = string_get(lexer->src, lexer->i++))) {
            case '0':
                string_push_char(str, '\0');
                break;
            case 'n':
                string_push_char(str, '\n');
                break;
            case 'r':
                string_push_char(str, '\r');
                break;
            case 't':
                string_push_char(str, '\t');
                break;
            case 'b':
                string_push_char(str, '\b');
                break;
            case 'f':
                string_push_char(str, '\f');
                break;
            case 'v':
                string_push_char(str, '\v');
                break;
            case '\n':
                break;
            default:
                string_push_char(str, c);
                break;
            }
        } else if (c == '"') break;
        else string_push_char(str, c);
    }

    return init_token(TK_STRING, str);
}

token_t lexer_next_token(lexer_t* self) {
    char c;
    while ((c = string_get(self->src, self->i++)) != -1) {
        if (!isascii(c)) {
            fprintf(stderr,
                "\033[31merror\033[39m: '%c' is a non-valid ascii character \n",
                c);

            continue;
        }
        else if (isblank(c) || isspace(c)) continue;
        else if (c == '/' && string_get(self->src, self->i) == '/') {
            c = string_get(self->src, self->i++);
            while (c != -1 && c != '\n') c = string_get(self->src, self->i++);
            continue;
        }
        else if (c == '"') return lexing_string(self);
        else if (isdigit(c)) return lexing_number(self, self->i - 1);
        else if (isalpha(c)) return lexing_ident(self, self->i - 1);
        else {
            fprintf(stderr,
                    "\033[31merror\033[39m: the character '%c' was invalid\n",
                    c);
            exit(1);
        }
    }

    return init_token(TK_EOF, NULL);
}

void lexer_drop(lexer_t* self) {
    string_drop(self->src);
    // free(self->filename);
    free(self);
}
