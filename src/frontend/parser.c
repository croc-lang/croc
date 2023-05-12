#include <stdarg.h>
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
    for (size_t i = 0; i < size; ++i) {
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

static path_type_t* parse_type_path(parser_t* self, string_t* first_segment) {
    vector_t* segments = new_vector();

    if (first_segment == NULL)
        first_segment = token_get_value(parser_eat(self, TK_IDENT), "");

    vector_push(segments, first_segment);

    while (parser_check(self, TK_DB_COLON)) {
        parser_eat(self, TK_DB_COLON);
        string_t* name = token_get_value(parser_eat(self, TK_IDENT), "");
        vector_push(segments, name);
    }

    return new_path_type(segments);
}

static arg_expr_t* parse_arg(parser_t* self) {
    type_t* type = parse_type(self, NULL);
    string_t* name = token_get_value(parser_eat(self, TK_IDENT), "");

    return new_arg_expr(type, name);
}

static expr_t* parse_primary_expr(parser_t* self) {
    expr_value_t value;
    token_t* token;
    expr_t* expr;

    switch (self->current->kind) {
    case TK_IDENT:
    case TK_STRING:
    case TK_INT:
    case TK_FLOAT:
        token = parser_advence(self);
        value.value = token->value;
        return new_expr(from_token(token->kind), value);
    case TK_LPAREN:
        parser_eat(self, TK_LPAREN);
        expr = parse_expr(self);
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
    string_t* name = token_get_value(parser_eat(self, TK_IDENT), "");

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
        stmt_t* stmt = parser_next(self);
        if (!stmt) break;
        vector_push(body, stmt);
        if (!parser_check(self, TK_COMMA)) break;
    }
    parser_eat(self, TK_LBRACE);

    stmt_value_t value = {.func = new_func_stmt(name, type, args, body)};
    return new_stmt(STMT_FUNC_DEFINITION, value);
}

static stmt_t* parse_var(parser_t* self, string_t* first_segment) {
    type_t* type = NULL;
    if (first_segment == NULL && parser_check(self, TK_KW_LET))
        parser_eat(self, TK_KW_LET);
    else
        type = parse_type(self, first_segment);

    expr_t* id = parse_expr(self);

    parser_eat(self, TK_EQ);

    expr_t* init = parse_expr(self);

    stmt_value_t value = {.var = new_var_stmt(false, type, id, init)};
    return new_stmt(STMT_VAR_DECLARATION, value);
}

static stmt_t* parse_const(parser_t* self) {
    type_t* type = NULL;
    parser_eat(self, TK_KW_CONST);
    if (parser_check(self, TK_KW_LET))
        parser_eat(self, TK_KW_LET);
    else
        type = parse_type(self, NULL);
    expr_t* id = parse_expr(self);

    parser_eat(self, TK_EQ);

    expr_t* init = parse_expr(self);

    stmt_value_t value = {.var = new_var_stmt(true, type, id, init)};
    return new_stmt(STMT_VAR_DECLARATION, value);
}

static stmt_t* parse_if(parser_t* self) {
    parser_eat(self, TK_KW_IF);
    parser_eat(self, TK_LPAREN);
    expr_t* condition = parse_expr(self);
    parser_eat(self, TK_RPAREN);

    vector_t* body = new_vector();
    if (parser_check(self, TK_LBRACE)) {
        parser_eat(self, TK_LBRACE);
        while (!parser_check(self, TK_RBRACE)) {
            vector_push(body, parser_next(self));
        }
        parser_eat(self, TK_RBRACE);
    } else vector_push(body, parser_next(self));

    stmt_value_t value = {.if_stmt = new_if_stmt(condition, body)};
    return new_stmt(STMT_IF, value);
}

type_t* parse_type(parser_t* self, string_t* first_segment) {
    type_t* type;

    type_value_t value;
    type_kind_t kind;
    expr_t* size = NULL;
    vector_t* generics = NULL;
    if (parser_check(self, TK_EOF)) return NULL;

    path_type_t* path = parse_type_path(self, first_segment);
    if (parser_check(self, TK_CMP_LT)) {
        parser_eat(self, TK_CMP_LT);
        while (!parser_check(self, TK_CMP_GT)) {
            vector_push(generics, parse_type(self, NULL));
            if (!parser_check(self, TK_COMMA)) break;
            parser_eat(self, TK_COMMA);
        }
        parser_eat(self, TK_CMP_GT);

        value.generic = new_generic_type(path, generics);
        kind = TY_GENERIC;
    } else {
        value.path = path;
        kind = TY_PATH;
    }
    type = new_type(kind, value);

    while (parser_checks(self, TK_STAR, TK_BIN_AND, TK_LBRACKET)) {
        token_kind_t token_kind = parser_advence(self)->kind;
        switch (token_kind) {
        case TK_STAR:
            value.type = type;
            kind = TY_POINTER;
            break;
        case TK_BIN_AND:
            value.type = type;
            kind = TY_REFERENCE;
            break;
        case TK_LBRACKET:
            parser_eat(self, TK_RBRACKET);
            value.type = type;
            kind = TY_SLICE;
            break;
        }
        type = new_type(kind, value);
    }

    return type;
}

expr_t* parse_expr(parser_t* self) {
    if (parser_check(self, TK_EOF)) return NULL;

    return parse_additive_expr(self);
}

stmt_t* parser_next(parser_t* self) {
    stmt_value_t value;

    switch (self->current->kind) {
    case TK_EOF: return NULL;
    case TK_KW_FUNC: return parse_func(self);
    case TK_KW_LET: return parse_var(self, NULL);
    case TK_KW_CONST: return parse_const(self);
    case TK_KW_IF: return parse_if(self);
    default:
        value.expr = parse_expr(self);
        if (parser_check(self, TK_SEMICOLON)) {
            parser_eat(self, TK_SEMICOLON);
            return new_stmt(STMT_EXPR, value);
        }

        return parse_var(self, value.expr->value.value);
    }
}

void parser_drop(parser_t* self) {
    lexer_drop(self->lexer);
    free(self);
}
