#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "../string_utils.h"

#define INC_AND_RETURN(lexer, kind) { \
    lexer_forward(lexer); \
    return new_token(kind, NULL, new_location(line, col, start, lexer->i)); \
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

    lexer->context = new_context();
    lexer->filename = path;
    lexer->src = src;
    lexer->i = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->prev_col = 0;

    fclose(file);

    return lexer;
}

lexer_t* new_lexer(char* path, char* src) {
    lexer_t* lexer = calloc(sizeof(lexer_t), 1);
    string_t* string_src = new_string(src);

    if (string_src == NULL)
        return NULL;

    lexer->context = new_context();
    lexer->filename = path;
    lexer->src = string_src;
    lexer->i = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->prev_col = 0;

    return lexer;
}

static size_t lexer_forward(lexer_t* self) {
    size_t index = self->i++;
    self->prev_col = self->col;
    if (string_get(self->src, self->i) == '\n') {
        self->col = 1;
        self->line += 1;
    } else self->col += 1;
    return index;
}

static size_t lexer_backward(lexer_t* self) {
    if (self->col == 1) self->line -= 1;
    self->col = self->prev_col;
    return --self->i;
}

static token_t* lexing_ident(lexer_t* lexer, size_t start) {
    size_t line, col = (lexer->line, lexer->col);
    while (isalnum(string_get(lexer->src, lexer_forward(lexer))));

    string_t* ident = string_slice(lexer->src, start, lexer_backward(lexer));

    if (string_eq_str(ident, "let")) {
        string_drop(ident);
        return new_token(TK_KW_LET,
            NULL,
            new_location(line, col, start, lexer->i));
    } else if (string_eq_str(ident, "if")) {
        string_drop(ident);
        return new_token(TK_KW_IF,
            NULL,
            new_location(line, col, start, lexer->i));
    } else if (string_eq_str(ident, "func")) {
        string_drop(ident);
        return new_token(TK_KW_FUNC,
            NULL,
            new_location(line, col, start, lexer->i));
    } else return new_token(TK_IDENT,
        ident,
        new_location(line, col, start, lexer->i));
}

static token_t* lexing_number(lexer_t* lexer, size_t start) {
    size_t line, col, first_occ = (lexer->line, lexer->col, 0);
    token_kind_t kind = TK_INT;
    bool error = false;
    string_t* slice;
    char c;
    while ((c = string_get(lexer->src, lexer_forward(lexer)), isalnum(c))) {
        if (isalpha(c) && !error) {
            error = true;
            first_occ = lexer->i - 1;
        }
    }

    c = string_get(lexer->src, lexer->i - 1);
    if (c > 0 && c == '.' && isdigit(string_get(lexer->src, lexer->i))) {
        kind = TK_FLOAT;
        while ((c = string_get(lexer->src, lexer_forward(lexer)), isalnum(c))) {
            if (isalpha(c) && !error) {
                error = true;
                first_occ = lexer->i - 1;
            }
        }
    }

    if (error) {
        slice = string_slice(lexer->src,
            start,
            lexer_backward(lexer));
        context_add_error(lexer->context,
            new_error(CTX_ERR_INVALID_SYNTAX,
                format_string("`%s` is invalid"
                    ", we didn't expect to have '%c' in the number"
                    ", try rewriting it.",
                    slice->data,
                    string_get(lexer->src, first_occ)),
                new_location(line, col, start, lexer->i)));
        string_drop(slice);
    }

    return new_token(kind,
        string_slice(lexer->src, start, lexer_backward(lexer)),
        new_location(line, col, start, lexer->i));
}

static token_t* lexing_string(lexer_t* lexer, size_t start) {
    size_t line, col = (lexer->line, lexer->col);
    string_t* str = new_string("");
    char c;

    while ((c = string_get(lexer->src, lexer_forward(lexer))) > 0) {
        if (c == '\\') {
            switch ((c = string_get(lexer->src, lexer_forward(lexer)))) {
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

    if (c != '"') context_add_error(lexer->context,
        new_error(CTX_ERR_UNTERMINATED_STR,
            new_string("not ended string"),
            new_location(line, col, start, lexer->i)));

    return new_token(TK_STRING, str, new_location(line, col, start, lexer->i));
}

token_t* lexer_next_token(lexer_t* self) {
    size_t line, col, start;
    char next_c, c;
    while ((c = string_get(self->src, lexer_forward(self))) > 0) {
        line, col, start = (self->line, self->col, self->i - 1);
        next_c = string_get(self->src, self->i);

        if (!isascii(c)) {
            while (!isascii(string_get(self->src, lexer_forward(self))));
            string_t* slice = string_slice(self->src, start,
                lexer_backward(self));

            context_add_error(self->context,
                new_error(CTX_ERR_INVALID_SLICE,
                    format_string("`%s` was invalid", slice->data),
                    new_location(line, col, start, self->i)));
            string_drop(slice);
            continue;
        } else if (isblank(c) || isspace(c)) continue;
        else if (isdigit(c)) return lexing_number(self, self->i - 1);
        else if (isalpha(c)) return lexing_ident(self, self->i - 1);
        else if (c == '"') return lexing_string(self, self->i - 1);
        else if (c == '/' && next_c == '/') {
            do { c = string_get(self->src, lexer_forward(self)); }
            while (c != -1 && c != '\n');
            continue;
        } else if (next_c == '=') {
            lexer_forward(self);
            switch (c) {
            case '=': return new_token(TK_CMP_EQ,
                NULL,
                new_location(line, col, start, self->i));
            case '!': return new_token(TK_CMP_NE,
                NULL,
                new_location(line, col, start, self->i));
            case '>': return new_token(TK_CMP_GE,
                NULL,
                new_location(line, col, start, self->i));
            case '<': return new_token(TK_CMP_LE,
                NULL,
                new_location(line, col, start, self->i));
            // TODO(hana) += -= ...
            default: context_add_error(self->context,
                new_error(CTX_ERR_INVALID_TOKEN,
                    format_string("the token `%c=` was invalid", c),
                    new_location(line, col, start, self->i)));
            }
        } else if (c == '<' && next_c == '<') INC_AND_RETURN(self, TK_BIT_SL)
        else if (c == '>' && next_c == '>') INC_AND_RETURN(self, TK_BIT_SR)
        else if (c == '&' && next_c == '&') INC_AND_RETURN(self, TK_BOOL_AND)
        else if (c == '|' && next_c == '|') INC_AND_RETURN(self, TK_BOOL_OR)
        else if (c == ':' && next_c == ':') INC_AND_RETURN(self, TK_DB_COLON)
        else if (c == '-' && next_c == '-') INC_AND_RETURN(self, TK_DECREMENT)
        else if (c == '+' && next_c == '+') INC_AND_RETURN(self, TK_INCREMENT)
        else if (c == '-' && next_c == '>') INC_AND_RETURN(self, TK_ARROW)
        else if (c == '+') return new_token(TK_PLUS,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '-') return new_token(TK_MINUS,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '*') return new_token(TK_STAR,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '/') return new_token(TK_SLASH,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '%') return new_token(TK_PERCENT,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '&') return new_token(TK_BIN_AND,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '|') return new_token(TK_BIN_OR,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '^') return new_token(TK_BIN_XOR,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '=') return new_token(TK_EQ,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '!') return new_token(TK_BANG,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '~') return new_token(TK_TILDE,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '<') return new_token(TK_CMP_LT,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '>') return new_token(TK_CMP_GT,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == ';') return new_token(TK_SEMICOLON,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == ',') return new_token(TK_COMMA,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '(') return new_token(TK_LPAREN,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == ')') return new_token(TK_RPAREN,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '{') return new_token(TK_LBRACE,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '}') return new_token(TK_RBRACE,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == '[') return new_token(TK_LBRACKET,
            NULL,
            new_location(line, col, start, self->i));
        else if (c == ']') return new_token(TK_RBRACKET,
            NULL,
            new_location(line, col, start, self->i));
        else {
            context_add_error(self->context,
                new_error(CTX_ERR_INVALID_CHAR,
                    format_string("the character '%c' was invalid", c),
                    new_location(line, col, start, self->i)));
        }
    }

    return new_token(TK_EOF,
        NULL,
        new_location(line, col, self->src->len, self->src->len));
}

void lexer_drop(lexer_t* self) {
    string_drop(self->src);
    free(self);
}
