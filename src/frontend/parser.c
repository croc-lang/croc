#include <stdlib.h>
#include "parser.h"

#define PARSE_OP_EXPR(name, calle, size, ...) \
static expr_t* name(parser_t* self) { \
    expr_t* left = calle(self); \
    while (parser_checks(self, size, __VA_ARGS__)) { \
        token_kind_t token_kind = parser_advence(self)->kind; \
        expr_kind_t op = from_token(token_kind); \
        expr_value_t value = {.binary = new_binary_expr(left, calle(self))}; \
        left = new_expr(op, value); \
    } \
    return left; \
}

parser_t* new_parser(lexer_t* lexer) {
    parser_t* parser = malloc(sizeof(parser_t));
    parser->context = lexer->context;
    parser->lexer = lexer;
    parser->current = lexer_next_token(lexer);
    return parser;
}

inline static token_t* parser_advence(parser_t* self) {
    token_t* prev = self->current;
    self->current = lexer_next_token(self->lexer);
    return prev;
}

inline static bool parser_check(parser_t* self, token_kind_t kind) {
    return self->current->kind == kind;
}

inline static bool parser_checks(parser_t* self, size_t size, ...) {
    va_list args;
    va_start(args, size);
    for (int i = 0; i < size; ++i) {
        if (parser_check(self, va_arg(args, token_kind_t))) return true;
    }
    va_end(args);
    return false;
}

inline static token_t* parser_eat(parser_t* self, token_kind_t kind) {
    if (!parser_check(self, kind)) {
        context_add_error(self->context,
            new_error(CTX_ERR_INVALID_TOKEN,
                format_string("`%s` was invalid", self->current->value->data),
                self->current->location));
    }
    token_t* prev = self->current;
    self->current = lexer_next_token(self->lexer);
    return prev;
}

static arg_expr_t* parse_arg(parser_t* self) {
    expr_t* type = parse_expr(self);
    string_t* name = parser_eat(self, TK_IDENT)->value;

    return new_arg_expr(type, name);
}

static expr_t* parse_primary_expr(parser_t* self) {
    switch (self->current->kind) {
    case TK_IDENT:
    case TK_STRING:
    case TK_INT:
    case TK_FLOAT: {
        token_t* token = parser_advence(self);
        expr_value_t value = {.value = token->value};
        return new_expr(from_token(token->kind), value);
    }
    case TK_LPAREN:
        parser_eat(self, TK_LPAREN);
        expr_t* expr = parse_expr(self);
        parser_eat(self, TK_RPAREN);
        return expr;
    default:
        break;
    }
}

static expr_t* parse_unary_expr(parser_t* self) {
    expr_kind_t kind = EX_NONE;
    expr_t* expr = NULL;

    if (parser_checks(self,
        5,
        TK_MINUS,
        TK_BANG,
        TK_TILDE,
        TK_INCREMENT,
        TK_DECREMENT)) {
        token_t* token = parser_advence(self);
        expr_value_t value = {.unary = new_unary_expr(parse_unary_expr(self))};
        switch (token->kind) {
        case TK_MINUS:
            kind = EX_UNA_NEG;
            break;
        case TK_BANG:
            kind = EX_UNA_NOT;
            break;
        case TK_TILDE:
            kind = EX_UNA_BINNOT;
            break;
        case TK_INCREMENT:
            kind = EX_UNA_INCR;
            break;
        case TK_DECREMENT:
            kind = EX_UNA_DECR;
            break;
        }
        return new_expr(kind, value);
    }

    expr = parse_primary_expr(self);
    while (parser_checks(self, 2, TK_INCREMENT, TK_DECREMENT)) {
        token_t* token = parser_advence(self);
        expr_value_t value = {.unary = new_unary_expr(expr)};
        switch (token->kind) {
            case TK_INCREMENT:
                kind = EX_UNA_SUFFIX_INCR;
                break;
            case TK_DECREMENT:
                kind = EX_UNA_SUFFIX_DECR;
                break;
        }
        expr = new_expr(kind, value);
    }
    return expr;
}

PARSE_OP_EXPR(
    parse_multiplicative_expr,
    parse_unary_expr,
    3,
    TK_STAR,
    TK_SLASH,
    TK_PERCENT
)
PARSE_OP_EXPR(
    parse_additive_expr,
    parse_multiplicative_expr,
    2,
    TK_PLUS,
    TK_MINUS
)

static stmt_t* parse_func(parser_t* self) {
    parser_eat(self, TK_KW_FUNC);  // skip the func kw
    string_t* name = parser_eat(self, TK_IDENT)->value;

    vector_t* args = new_vector();
    parser_eat(self, TK_RPAREN);
    while (!parser_check(self, TK_LPAREN)) {
        vector_push(args, parse_arg(self));
        if (!parser_check(self, TK_COMMA)) break;
    }
    parser_eat(self, TK_LPAREN);

    expr_t* type = parse_expr(self);

    vector_t* body = new_vector();
    parser_eat(self, TK_RBRACE);
    while (!parser_check(self, TK_LBRACE)) {
        vector_push(body, parser_next(self));
        if (!parser_check(self, TK_COMMA)) break;
    }
    parser_eat(self, TK_LBRACE);

    stmt_value_t value = {.func = new_func_stmt(name, type, args, body)};
    return new_stmt(STMT_FUNC_DEFINITION, value);
}

expr_t* parse_expr(parser_t* self) {
    return parse_additive_expr(self);
}

stmt_t* parser_next(parser_t* self) {
    switch (self->current->kind) {
    case TK_KW_FUNC: return parse_func(self);
    default: {
        stmt_value_t value = {.expr = parse_expr(self)};
        parser_eat(self, TK_SEMICOLON);
        return new_stmt(STMT_EXPR, value);
    }
    }
}

void parser_drop(parser_t* self) {
    lexer_drop(self->lexer);
    free(self);
}
