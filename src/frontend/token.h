#ifndef FRONTEND_TOKEN_H
#define FRONTEND_TOKEN_H
#include <stdio.h>
#include "../string_utils.h"

typedef enum TokenKind {
    TK_IDENT,
    TK_INT,
    TK_FLOAT,
    TK_EOF,
} token_kind_t;

typedef struct Token {
    token_kind_t kind;
    string_t* value;
} token_t;

typedef struct Location {
    size_t line;
    size_t col;
    size_t pos;
    size_t size;
} location_t;

token_t init_token(token_kind_t kind, string_t* value);
void token_drop(token_t* self);
#endif
