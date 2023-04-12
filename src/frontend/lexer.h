#ifndef FRONTEND_LEXER_H
#define FRONTEND_LEXER_H
#include "token.h"
#include "../string_utils.h"
#include "../context.h"

typedef struct Lexer {
    context_t* context;

    char* filename;
    string_t* src;
    size_t i;
    size_t line;
    size_t col;
    size_t prev_col;
} lexer_t;

lexer_t* from_file_lexer(char* path);
lexer_t* new_lexer(char* path, char* src);
token_t lexer_next_token(lexer_t* self);
void lexer_drop(lexer_t* self);
#endif
