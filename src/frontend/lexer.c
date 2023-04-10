#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "../string_utils.h"

#define INC_AND_RETURN(lexer, kind) { \
    lexer->i++;                       \
    return init_token(kind, NULL);    \
}

lexer_t* from_file_lexer(char* path) {
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

lexer_t* new_lexer(char* path, char* src) {
    lexer_t* lexer = calloc(sizeof(lexer_t), 1);
    string_t* string_src = new_string(src);

    if (string_src == NULL)
        return NULL;

    lexer->filename = path;
    lexer->src = string_src;
    lexer->i = 0;

    return lexer;
}

static token_t lexing_ident(lexer_t* lexer, size_t start) {
    while (isalnum(string_get(lexer->src, lexer->i++)));

    string_t* ident = string_slice(lexer->src, start, --lexer->i);

    if (string_eq(ident, "let")) {
        string_drop(ident);
        return init_token(TK_KW_LET, NULL);
    } else if (string_eq(ident, "if")) {
        string_drop(ident);
        return init_token(TK_KW_IF, NULL);
    } else if (string_eq(ident, "func")) {
        string_drop(ident);
        return init_token(TK_KW_FUNC, NULL);
    } else return init_token(TK_IDENT, ident);
}

static token_t lexing_number(lexer_t* lexer, size_t start) {
    while (isdigit(string_get(lexer->src, lexer->i++)));

    char c = string_get(lexer->src, lexer->i - 1);
    if (c > 0 && c == '.' && isdigit(string_get(lexer->src, lexer->i))) {
        while (isdigit(string_get(lexer->src, lexer->i++)));

        return init_token(TK_FLOAT,
            string_slice(lexer->src, start, lexer->i - 1));
    }

    return init_token(TK_INT, string_slice(lexer->src, start, --lexer->i));
}

static token_t lexing_string(lexer_t* lexer) {
    string_t* str = new_string("");
    char c;

    while ((c = string_get(lexer->src, lexer->i++)) > 0) {
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
    char next_c;
    char c;
    while ((c = string_get(self->src, self->i++)) > 0) {
        next_c = string_get(self->src, self->i);

        if (!isascii(c)) {
            fprintf(stderr,
                "\033[31merror\033[39m: '%c' is a non-valid ascii character \n",
                c);

            continue;
        } else if (isblank(c) || isspace(c)) continue;
        else if (isdigit(c)) return lexing_number(self, self->i - 1);
        else if (isalpha(c)) return lexing_ident(self, self->i - 1);
        else if (c == '"') return lexing_string(self);
        else if (c == '/' && next_c == '/') {
            do { c = string_get(self->src, self->i++); }
            while (c != -1 && c != '\n');
            continue;
        } else if (next_c == '=') {
            self->i++;
            switch (c) {
            case '=': return init_token(TK_CMP_EQ, NULL);
            case '!': return init_token(TK_CMP_NE, NULL);
            case '>': return init_token(TK_CMP_GE, NULL);
            case '<': return init_token(TK_CMP_LE, NULL);
            // TODO(hana) += -= ...
            default:
                fprintf(stderr,
                    "\033[31merror\033[39m: the character '%c' was invalid\n",
                    c);
                exit(1);
            }
        } else if (c == '<' && next_c == '<') INC_AND_RETURN(self, TK_BIT_SL)
        else if (c == '>' && next_c == '>') INC_AND_RETURN(self, TK_BIT_SR)
        else if (c == '&' && next_c == '&') INC_AND_RETURN(self, TK_BOOL_AND)
        else if (c == '|' && next_c == '|') INC_AND_RETURN(self, TK_BOOL_OR)
        else if (c == ':' && next_c == ':') INC_AND_RETURN(self, TK_DB_COLON)
        else if (c == '-' && next_c == '-') INC_AND_RETURN(self, TK_DECREMENT)
        else if (c == '+' && next_c == '+') INC_AND_RETURN(self, TK_INCREMENT)
        else if (c == '-' && next_c == '>') INC_AND_RETURN(self, TK_ARROW)
        else if (c == '+') return init_token(TK_PLUS, NULL);
        else if (c == '-') return init_token(TK_MINUS, NULL);
        else if (c == '*') return init_token(TK_STAR, NULL);
        else if (c == '/') return init_token(TK_SLASH, NULL);
        else if (c == '%') return init_token(TK_PERCENT, NULL);
        else if (c == '&') return init_token(TK_BIN_AND, NULL);
        else if (c == '|') return init_token(TK_BIN_OR, NULL);
        else if (c == '^') return init_token(TK_BIN_XOR, NULL);
        else if (c == '=') return init_token(TK_EQ, NULL);
        else if (c == '!') return init_token(TK_BANG, NULL);
        else if (c == '~') return init_token(TK_TILDE, NULL);
        else if (c == '<') return init_token(TK_CMP_LT, NULL);
        else if (c == '>') return init_token(TK_CMP_GT, NULL);
        else if (c == ';') return init_token(TK_SEMICOLON, NULL);
        else if (c == ',') return init_token(TK_COMMA, NULL);
        else if (c == '(') return init_token(TK_LPAREN, NULL);
        else if (c == ')') return init_token(TK_RPAREN, NULL);
        else if (c == '{') return init_token(TK_LBRACE, NULL);
        else if (c == '}') return init_token(TK_RBRACE, NULL);
        else if (c == '[') return init_token(TK_LBRACKET, NULL);
        else if (c == ']') return init_token(TK_RBRACKET, NULL);
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
