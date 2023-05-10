#ifndef FRONTEND_PARSER_H
#define FRONTEND_PARSER_H
#include "statement.h"
#include "lexer.h"

typedef struct Parser {
    context_t* context;

    lexer_t* lexer;
    token_t* current;
} parser_t;

parser_t* new_parser(lexer_t* lexer);
stmt_t* parser_next(parser_t* self);
expr_t* parse_expr(parser_t* self);
type_t* parse_type(parser_t* self, string_t* first_segment);
void parser_drop(parser_t* self);
#endif
