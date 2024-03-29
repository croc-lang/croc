#ifndef FRONTEND_TOKEN_H
#define FRONTEND_TOKEN_H
#include <stdio.h>
#include <stdbool.h>
#include "../string_utils.h"
#include "../location.h"

typedef enum TokenKind {
    TK_KW_LET,
    TK_KW_IF,
    TK_KW_ELSE,
    TK_KW_FUNC,
    TK_KW_CONST,
    TK_KW_FOR,
    TK_KW_WHILE,
    TK_KW_PUBLIC,
    TK_KW_IMPORT,
    TK_KW_MODULE,
    TK_KW_RETURN,
    TK_KW_AS,
    TK_KW_IN,
    TK_KW_STRUCT,

    TK_LPAREN,        // (
    TK_RPAREN,        // )
    TK_LBRACE,        // {
    TK_RBRACE,        // }
    TK_LBRACKET,      // [
    TK_RBRACKET,      // ]
    TK_COMMA,         // ,
    TK_SEMICOLON,     // ;
    TK_DB_COLON,      // ::
    TK_ARROW,         // ->
    TK_PLUS,          // +
    TK_MINUS,         // -
    TK_STAR,          // *
    TK_SLASH,         // /
    TK_PERCENT,       // %
    TK_BOOL_OR,       // ||
    TK_BOOL_AND,      // &&
    TK_BIN_OR,        // |
    TK_BIN_AND,       // &
    TK_BIN_XOR,       // ^
    TK_CMP_EQ,        // ==
    TK_BANG,          // !
    TK_TILDE,         // ~
    TK_CMP_NE,        // !=
    TK_CMP_LT,        // <
    TK_CMP_GT,        // >
    TK_CMP_LE,        // <=
    TK_CMP_GE,        // >=
    TK_BIT_SL,        // <<
    TK_BIT_SR,        // >>
    TK_INCREMENT,     // ++
    TK_DECREMENT,     // --
    TK_EQ,            // =
    TK_ASG_BIT_SL,    // <<=
    TK_ASG_BIT_SR,    // >>=
    TK_ASG_BOOL_AND,  // &&=
    TK_ASG_BOOL_OR,   // ||=
    TK_ASG_PLUS,      // +=
    TK_ASG_MINUS,     // -=
    TK_ASG_STAR,      // *=
    TK_ASG_SLASH,     // /=
    TK_ASG_PERCENT,   // %=
    TK_ASG_BIT_AND,   // &=
    TK_ASG_BIT_OR,    // |=
    TK_ASG_BIT_XOR,   // ^=

    TK_IDENT,
    TK_STRING,
    TK_INT,
    TK_FLOAT,
    TK_EOF,
} token_kind_t;

typedef struct Token {
    token_kind_t kind;
    string_t* value;
    location_t* location;
} token_t;

token_t* new_token(token_kind_t kind, string_t* value, location_t* location);
string_t* token_get_value(token_t* self, char* or_default);
bool token_check(token_t* self, token_t* other);
void token_drop(token_t* self);
#endif
