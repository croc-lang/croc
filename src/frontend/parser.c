#include <stdlib.h>
#include "./parser.h"

parser_t* new_parser(lexer_t* lexer) {
    parser_t* parser = malloc(sizeof(parser_t));
    parser->lexer = lexer;
    parser->current = lexer_next_token(lexer);
}

static void parser_advence(parser_t* self) {
    self->current = lexer_next_token(self->lexer);
}

static token_t parser_eat(parser_t* self, token_kind_t kind) {
    if (self->current.kind != kind) {
        printf("error: unexpected token");
        exit(1);
    }
    token_t prev = self->current;
    self->current = lexer_next_token(self->lexer);
    return prev;
}

static arg_expr_t* parse_arg(parser_t* self) {
    expr_t* type = parse_expr(self);
    string_t* name = parser_eat(self, TK_IDENT).value;

    return new_arg_expr(type, name);
}

static expr_t* parse_expr(parser_t* self) {
    // TODO(hana)
}

static stmt_t* parse_stmt(parser_t* self) {
    // TODO(hana)
}

static stmt_t* parse_func(parser_t* self) {
    parser_eat(self, TK_KW_FUNC); // skip the func kw
    string_t* name = parser_eat(self, TK_IDENT).value;

    vector_t* args = new_vector();
    parser_eat(self, TK_RPAREN);
    while (self->current.kind != TK_LPAREN) {
        vector_push(args, parse_arg(self));
        if (self->current.kind != TK_COMMA) break;
    }
    parser_eat(self, TK_LPAREN);

    expr_t* type = parse_expr(self);

    vector_t* body = new_vector();
    parser_eat(self, TK_RBRACE);
    while (self->current.kind != TK_LBRACE) {
        vector_push(body, parse_stmt(self));
        if (self->current.kind != TK_COMMA) break;
    }
    parser_eat(self, TK_LBRACE);

    stmt_value_t value = {.func = new_func_stmt(name, type, args, body)};
    return new_stmt(STMT_FUNC_DEFINITION, value);
}

stmt_t* parser_next(parser_t* self) {
    switch (self->current.kind) {
    case TK_KW_FUNC: return parse_func(self);
    default:
        break;
    }
}
