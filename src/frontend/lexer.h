#ifndef FRONTEND_LEXER_H
#define FRONTEND_LEXER_H
#include "token.h"
#include "../string_utils.h"

typedef struct Lexer {
    char* filename;
    string_t* src;
    unsigned int i;
} lexer_t;

lexer_t* from_file_lexer(char* path);
lexer_t* new_lexer(char* path, char* src);
token_t lexer_next_token(lexer_t* self);
void lexer_drop(lexer_t* self);
#endif
