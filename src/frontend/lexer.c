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

    return init_token(TK_INT, string_slice(lexer->src, start, lexer->i - 1));
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
        else if (isdigit(c)) return lexing_number(self, self->i - 1);
        else if (isalpha(c)) return lexing_ident(self, self->i - 1);

        fprintf(stderr,
                "\033[31merror\033[39m: the character '%c' was invalid\n", c);
        exit(1);
    }

    return init_token(TK_EOF, NULL);
}

void lexer_drop(lexer_t* self) {
    string_drop(self->src);
    // free(self->filename);
    free(self);
}
