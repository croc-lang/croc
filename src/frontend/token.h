#ifndef FRONTEND_TOKEN_H
#define FRONTEND_TOKEN_H
#include <stdio.h>
#include "../string_utils.h"

typedef enum TokenKind {
    TK_KW_LET,
    TK_KW_IF,
    TK_KW_FUNC,

    TK_LPAREN,     // (
    TK_RPAREN,     // )
    TK_LBRACE,     // {
    TK_RBRACE,     // }
    TK_LBRACKET,   // [
    TK_RBRACKET,   // ]
    TK_COMMA,      // ,
    TK_SEMICOLON,  // ;
    TK_DB_COLON,   // ::
    TK_ARROW,      // ->
    TK_PLUS,       // +
    TK_MINUS,      // -
    TK_STAR,       // *
    TK_SLASH,      // /
    TK_PERCENT,    // %
    TK_BOOL_OR,    // ||
    TK_BOOL_AND,   // &&
    TK_BIN_OR,     // |
    TK_BIN_AND,    // &
    TK_BIN_XOR,    // ^
    TK_EQ,         // =
    TK_CMP_EQ,     // ==
    TK_BANG,       // !
    TK_TILDE,      // ~
    TK_CMP_NE,     // !=
    TK_CMP_LT,     // <
    TK_CMP_GT,     // >
    TK_CMP_LE,     // <=
    TK_CMP_GE,     // >=
    TK_BIT_SL,     // <<
    TK_BIT_SR,     // >>

    TK_IDENT,
    TK_STRING,
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
