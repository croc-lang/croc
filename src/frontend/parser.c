#include <stdarg.h>
#include <stdlib.h>
#include <frontend/parser.h>

static else_branch_stmt_t* parse_else_branch(parser_t* self);
static if_stmt_t* parse_if_branch(parser_t* self);

#define PARSE_OP_EXPR(name, calle, size, ...) \
static expr_t* name(parser_t* self) { \
    expr_t* left = calle(self); \
    token_t* token = NULL; \
    expr_value_t value; \
    expr_kind_t op; \
    while (parser_checks(self, size, __VA_ARGS__)) { \
        token = parser_advence(self); \
        op = from_token(token->kind); \
        value.binary = new_binary_expr(left, calle(self)); \
        left = new_expr(op, value); \
        token_drop(token); \
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

inline static bool parser_skip_if_exist(parser_t* self, token_kind_t kind) {
    if (!parser_check(self, kind))
        return false;

    token_drop(self->current);
    self->current = lexer_next_token(self->lexer);
    return true;
}

inline static void parser_eat(parser_t* self, token_kind_t kind) {
    if (!parser_check(self, kind)) {
        context_add_error(self->context,
            new_error(CTX_ERR_INVALID_TOKEN,
                format_string(
                    "`%s` was invalid",
                    token_get_value(self->current, "")),
                location_clone(self->current->location)));
    }
    token_drop(self->current);
    self->current = lexer_next_token(self->lexer);
}

static path_type_t* parse_type_path(parser_t* self) {
    vector_t* segments = new_vector();
    string_t* name = token_get_value(self->current, "");

    vector_push(segments, name);
    parser_eat(self, TK_IDENT);

    while (parser_skip_if_exist(self, TK_DB_COLON)) {
        name = token_get_value(self->current, "");
        parser_eat(self, TK_IDENT);
        vector_push(segments, name);
    }

    return new_path_type(segments);
}

static arg_expr_t* parse_arg(parser_t* self) {
    type_t* type = parse_type(self);
    string_t* name = token_get_value(self->current, "");
    parser_eat(self, TK_IDENT);

    return new_arg_expr(type, name);
}

static stmt_t* parse_module(parser_t* self) {
    stmt_value_t value;
    parser_eat(self, TK_KW_MODULE);

    value.module = new_module_stmt(parse_type_path(self));
    return new_stmt(STMT_MODULE, value);
}

static stmt_t* parse_import(parser_t* self) {
    vector_t* imports = new_vector();
    vector_t* import_objects;
    string_t* move_to;
    string_t* path;
    stmt_value_t value;

    parser_eat(self, TK_KW_IMPORT);
    bool in_paren = parser_skip_if_exist(self, TK_LPAREN);
    do {
        import_objects = NULL;
        move_to = NULL;
        path = token_get_value(self->current, "");
        parser_eat(self, TK_STRING);
        if (parser_skip_if_exist(self, TK_KW_AS)) {
            move_to = token_get_value(self->current, "");
            parser_eat(self, TK_IDENT);
        } else if (parser_skip_if_exist(self, TK_LBRACE)) {
            import_objects = new_vector();
            while (!parser_check(self, TK_RBRACE)) {
                vector_push(import_objects, token_get_value(self->current, ""));
                parser_eat(self, TK_IDENT);
                if (!parser_skip_if_exist(self, TK_COMMA)) break;
            }
            parser_eat(self, TK_RBRACE);
        }
        vector_push(imports, new_import_stmt(path, move_to, import_objects));
    } while (parser_skip_if_exist(self, TK_COMMA));

    if (in_paren) parser_eat(self, TK_RPAREN);

    value.imports = imports;
    return new_stmt(STMT_IMPORTS, value);
}

static expr_t* parse_primary_expr(parser_t* self) {
    vector_t* body = NULL;
    token_t* token = NULL;
    expr_t* expr = NULL;
    expr_value_t value;

    switch (self->current->kind) {
    case TK_STRING:
        token = parser_advence(self);
        value.value = string_clone(token->value);
        token_drop(token);
        while (parser_check(self, TK_STRING)) {
            token = parser_advence(self);
            string_push_str(value.value, token->value);
            token_drop(token);
        }
        expr = new_expr(EX_STRING_LITERAL, value);
        break;
    case TK_IDENT:
    case TK_INT:
    case TK_FLOAT:
        token = parser_advence(self);
        value.value = string_clone(token->value);
        expr = new_expr(from_token(token->kind), value);
        token_drop(token);
        break;
    case TK_LPAREN:
        parser_eat(self, TK_LPAREN);
        expr = parse_expr(self);
        if (parser_skip_if_exist(self, TK_COMMA)) {
            body = new_vector();
            vector_push(body, expr);
            while (!parser_check(self, TK_RPAREN)) {
                vector_push(body, parse_expr(self));
                if (!parser_skip_if_exist(self, TK_COMMA)) break;
            }
            value.list = body;
            expr = new_expr(EX_TUPLE, value);
        }
        parser_eat(self, TK_RPAREN);
        break;
    case TK_LBRACKET:
        body = new_vector();
        parser_eat(self, TK_LBRACKET);
        while (!parser_check(self, TK_RBRACKET)) {
            vector_push(body, parse_expr(self));
            if (!parser_skip_if_exist(self, TK_COMMA)) break;
        }
        parser_eat(self, TK_RBRACKET);
        value.list = body;
        expr = new_expr(EX_ARRAY, value);
        break;
    }

    return expr;
}

static expr_t* parse_unary_expr(parser_t* self) {
    expr_kind_t kind = EX_NONE;
    token_t* token = NULL;
    expr_t* expr = NULL;
    expr_value_t value;

    if (parser_checks(self,
        5,
        TK_MINUS,
        TK_BANG,
        TK_TILDE,
        TK_INCREMENT,
        TK_DECREMENT)) {
        token = parser_advence(self);
        value.unary = new_unary_expr(parse_unary_expr(self));
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
        token_drop(token);
        return new_expr(kind, value);
    }

    expr = parse_primary_expr(self);
    while (parser_checks(self, 2, TK_INCREMENT, TK_DECREMENT)) {
        token = parser_advence(self);
        value.unary = new_unary_expr(expr);
        switch (token->kind) {
            case TK_INCREMENT:
                kind = EX_UNA_SUFFIX_INCR;
                break;
            case TK_DECREMENT:
                kind = EX_UNA_SUFFIX_DECR;
                break;
        }
        token_drop(token);
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

static expr_t* parse_primary_declaration_expr(parser_t* self) {
    vector_t* body = NULL;
    expr_t* expr = NULL;
    expr_value_t value;

    switch (self->current->kind) {
    case TK_LPAREN:
        parser_eat(self, TK_LPAREN);
        expr = parse_expr(self);
        if (parser_skip_if_exist(self, TK_COMMA)) {
            body = new_vector();
            vector_push(body, expr);
            while (!parser_check(self, TK_RPAREN)) {
                vector_push(body, parse_expr(self));
                if (!parser_skip_if_exist(self, TK_COMMA)) break;
            }
            value.list = body;
            expr = new_expr(EX_TUPLE, value);
        }
        parser_eat(self, TK_RPAREN);
        break;
    default:
        value.value = token_get_value(self->current, "");
        expr = new_expr(from_token(self->current->kind), value);
        parser_eat(self, TK_IDENT);
    }

    return expr;
}

static stmt_t* parse_func(parser_t* self) {
    vector_t* args = new_vector();
    vector_t* body = new_vector();
    string_t* name = NULL;
    type_t* type = NULL;
    stmt_t* stmt = NULL;
    stmt_value_t value;

    parser_eat(self, TK_KW_FUNC);  // skip the func kw
    name = token_get_value(self->current, "");
    parser_eat(self, TK_IDENT);

    parser_eat(self, TK_LPAREN);
    while (!parser_check(self, TK_RPAREN)) {
        vector_push(args, parse_arg(self));
        if (!parser_skip_if_exist(self, TK_COMMA)) break;
    }
    parser_eat(self, TK_RPAREN);

    if (!parser_check(self, TK_LBRACE)) type = parse_type(self);

    parser_eat(self, TK_LBRACE);
    while (!parser_check(self, TK_RBRACE)) {
        stmt = parser_next(self);
        if (!stmt) break;
        vector_push(body, stmt);
    }
    parser_eat(self, TK_RBRACE);

    value.func = new_func_stmt(name, type, args, body);
    return new_stmt(STMT_FUNC_DEFINITION, value);
}

static var_stmt_t* parse_var_declaration(parser_t* self, bool constant) {
    type_t* type = NULL;
    expr_t* init = NULL;
    expr_t* id = NULL;
    if (constant) parser_eat(self, TK_KW_CONST);
    if (!parser_skip_if_exist(self, TK_KW_LET)) type = parse_type(self);

    id = parse_primary_declaration_expr(self);

    if (parser_skip_if_exist(self, TK_EQ)) init = parse_expr(self);

    return new_var_stmt(constant, type, id, init);
}

static else_branch_stmt_t* parse_else_branch(parser_t* self) {
    vector_t* body = NULL;
    stmt_t* stmt = NULL;
    parser_eat(self, TK_KW_ELSE);

    if (parser_check(self, TK_KW_IF))
        return new_else_branch_stmt(NULL, parse_if_branch(self));

    body = new_vector();
    if (parser_skip_if_exist(self, TK_LBRACE)) {
        while (!parser_check(self, TK_RBRACE)) {
            stmt = parser_next(self);
            if (!stmt) break;
            vector_push(body, stmt);
        }
        parser_eat(self, TK_RBRACE);
    } else vector_push(body, parser_next(self));

    return new_else_branch_stmt(body, NULL);
}

static if_stmt_t* parse_if_branch(parser_t* self) {
    vector_t* body = new_vector();
    else_branch_stmt_t* else_branch = NULL;
    expr_t* condition = NULL;
    stmt_t* stmt = NULL;

    parser_eat(self, TK_KW_IF);
    parser_eat(self, TK_LPAREN);
    condition = parse_expr(self);
    parser_eat(self, TK_RPAREN);

    if (parser_skip_if_exist(self, TK_LBRACE)) {
        while (!parser_check(self, TK_RBRACE)) {
            stmt = parser_next(self);
            if (!stmt) break;
            vector_push(body, stmt);
        }
        parser_eat(self, TK_RBRACE);
    } else vector_push(body, parser_next(self));

    if (parser_check(self, TK_KW_ELSE))
        else_branch = parse_else_branch(self);

    return new_if_stmt(condition, body, else_branch);
}

static stmt_t* parse_var(parser_t* self, bool constant) {
    stmt_value_t value = {.var = parse_var_declaration(self, constant)};
    return new_stmt(STMT_VAR_DECLARATION, value);
}

static stmt_t* parse_if(parser_t* self) {
    stmt_value_t value = {.if_stmt = parse_if_branch(self)};
    return new_stmt(STMT_IF, value);
}

static stmt_t* parse_while(parser_t* self) {
    vector_t* body = new_vector();
    expr_t* condition = NULL;
    stmt_t* stmt = NULL;
    stmt_value_t value;

    parser_eat(self, TK_KW_WHILE);
    parser_eat(self, TK_LPAREN);
    condition = parse_expr(self);
    parser_eat(self, TK_RPAREN);

    if (parser_skip_if_exist(self, TK_LBRACE)) {
        while (!parser_check(self, TK_RBRACE)) {
            stmt = parser_next(self);
            if (!stmt) break;
            vector_push(body, stmt);
        }
        parser_eat(self, TK_RBRACE);
    } else vector_push(body, parser_next(self));

    value.while_stmt = new_while_stmt(condition, body);
    return new_stmt(STMT_WHILE, value);
}

static each_for_t* parse_loop_for_each(parser_t* self, bool constant) {
    type_t* type = NULL;
    expr_t* init = NULL;
    expr_t* id = NULL;
    if (constant) parser_eat(self, TK_KW_CONST);
    if (!parser_skip_if_exist(self, TK_KW_LET)) type = parse_type(self);

    id = parse_primary_declaration_expr(self);
    parser_eat(self, TK_KW_IN);
    init = parse_expr(self);

    return new_each_for(constant, type, id, init);
}

static primary_for_t* parse_loop_for(parser_t* self, bool constant) {
    for_init_kind_t init_kind = FOR_INIT_NONE;
    expr_t* condition = NULL;
    expr_t* updater = NULL;
    for_init_value_t init;

    size_t errors_len = self->context->errors->len;
    location_t* start_position = NULL;

    if (!parser_check(self, TK_SEMICOLON)) {
        start_position = location_clone(self->current->location);
        if (parser_check(self, TK_IDENT) || parser_check(self, TK_LPAREN)) {
            type_drop(parse_type(self));
            expr_drop(parse_primary_declaration_expr(self));
            if (parser_checks(self, 2, TK_EQ, TK_SEMICOLON)) {
                lexer_goto_location(self->lexer, start_position);
                token_drop(parser_advence(self));
                init.decla = parse_var_declaration(self, false);
                init_kind = FOR_INIT_DECLA;
            }
        }

        if (init_kind == FOR_INIT_NONE) {
            context_forget_errors(
                self->context,
                self->context->errors->len - errors_len);
            lexer_goto_location(self->lexer, start_position);
            token_drop(parser_advence(self));

            init.expr = parse_expr(self);
            init_kind = FOR_INIT_EXPR;
        }
    }

    parser_eat(self, TK_SEMICOLON);
    if (!parser_check(self, TK_SEMICOLON)) condition = parse_expr(self);
    parser_eat(self, TK_SEMICOLON);
    if (!parser_check(self, TK_RPAREN)) updater = parse_expr(self);

    return new_primary_for(init, init_kind, condition, updater);
}

static stmt_t* parse_for(parser_t* self) {
    size_t errors_len = self->context->errors->len;
    location_t* start_position = NULL;
    vector_t* body = new_vector();
    for_value_t for_value;
    for_kind_t for_kind = FK_NONE;
    stmt_t* stmt = NULL;
    stmt_value_t value;

    bool constant;

    parser_eat(self, TK_KW_FOR);
    parser_eat(self, TK_LPAREN);
    if (parser_check(self, TK_SEMICOLON)) {
        for_value.primary = parse_loop_for(self, false);
        for_kind = FK_PRIMARY;
    } else {
        start_position = location_clone(self->current->location);
        constant = parser_skip_if_exist(self, TK_KW_CONST);
        if (parser_checks(self, 3, TK_KW_LET, TK_IDENT, TK_LPAREN)) {
            if (!parser_skip_if_exist(self, TK_KW_LET))
                type_drop(parse_type(self));
            expr_drop(parse_primary_declaration_expr(self));
            if (parser_check(self, TK_KW_IN)) {
                lexer_goto_location(self->lexer, start_position);
                token_drop(parser_advence(self));
                for_value.each = parse_loop_for_each(self, constant);
                for_kind = FK_EACH;
            }
        }

        if (for_kind == FK_NONE) {
            context_forget_errors(
                self->context,
                self->context->errors->len - errors_len);
            lexer_goto_location(self->lexer, start_position);
            token_drop(parser_advence(self));

            for_value.primary = parse_loop_for(self, constant);
            for_kind = FK_PRIMARY;
        }
    }
    parser_eat(self, TK_RPAREN);

    if (parser_skip_if_exist(self, TK_LBRACE)) {
        while (!parser_check(self, TK_RBRACE)) {
            stmt = parser_next(self);
            if (!stmt) break;
            vector_push(body, stmt);
        }
        parser_eat(self, TK_RBRACE);
    } else vector_push(body, parser_next(self));

    value.for_stmt = new_for_stmt(for_value, for_kind, body);
    return new_stmt(STMT_FOR, value);
}

static type_t* parse_type_tuple(parser_t* self) {
    vector_t* body = NULL;
    type_t* type = NULL;
    type_value_t value;

    parser_eat(self, TK_LPAREN);
    type = parse_type(self);
    if (parser_skip_if_exist(self, TK_COMMA)) {
        body = new_vector();
        vector_push(body, type);
        while (!parser_check(self, TK_RPAREN)) {
            vector_push(body, parse_type(self));
            if (!parser_skip_if_exist(self, TK_COMMA)) break;
        }
        value.tuple = body;
        type = new_type(TY_TUPLE, value);
    }
    parser_eat(self, TK_RPAREN);

    return type;
}

static stmt_t* parse_return(parser_t* self) {
    parser_eat(self, TK_KW_RETURN);
    stmt_value_t value = {.expr = parse_expr(self)};
    return new_stmt(STMT_RETURN, value);
}

type_t* parse_type(parser_t* self) {
    vector_t* generics = NULL;
    token_t* token = NULL;
    type_t* type = NULL;
    type_value_t value;
    type_kind_t kind;

    if (parser_check(self, TK_LPAREN)) type = parse_type_tuple(self);
    else {
        path_type_t* path = parse_type_path(self);
        if (parser_skip_if_exist(self, TK_CMP_LT)) {
            while (!parser_check(self, TK_CMP_GT)) {
                vector_push(generics, parse_type(self));
                if (!parser_skip_if_exist(self, TK_COMMA)) break;
            }
            parser_eat(self, TK_CMP_GT);

            value.generic = new_generic_type(path, generics);
            kind = TY_GENERIC;
        } else {
            value.path = path;
            kind = TY_PATH;
        }
        type = new_type(kind, value);
    }

    while (parser_checks(self, 3, TK_STAR, TK_BIN_AND, TK_LBRACKET)) {
        token = parser_advence(self);
        switch (token->kind) {
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
        token_drop(token);
        type = new_type(kind, value);
    }

    return type;
}

expr_t* parse_expr(parser_t* self) {
    return parse_additive_expr(self);
}

stmt_t* parser_next(parser_t* self) {
    size_t errors_len = self->context->errors->len;
    location_t* start_position = NULL;
    token_t* token = NULL;
    stmt_t* stmt = NULL;
    stmt_value_t value;

    switch (self->current->kind) {
    case TK_EOF: break;
    case TK_SEMICOLON:
        parser_eat(self, TK_SEMICOLON);
        stmt = parser_next(self);
        break;
    case TK_KW_PUBLIC:
        parser_eat(self, TK_KW_PUBLIC);
        if (parser_check(self, TK_KW_FUNC)) {
            stmt = parse_func(self);
            stmt->value.func->public = true;
        } else {
            stmt = parse_var(self, parser_check(self, TK_KW_CONST));
            stmt->value.var->public = true;
        }
        break;
    case TK_KW_RETURN:
        stmt = parse_return(self);
        parser_eat(self, TK_SEMICOLON);
        break;
    case TK_KW_IMPORT:
        stmt = parse_import(self);
        break;
    case TK_KW_MODULE:
        stmt = parse_module(self);
        break;
    case TK_KW_FUNC:
        stmt = parse_func(self);
        break;
    case TK_KW_LET:
    case TK_KW_CONST:
        stmt = parse_var(self, parser_check(self, TK_KW_CONST));
        parser_eat(self, TK_SEMICOLON);
        break;
    case TK_KW_IF:
        stmt = parse_if(self);
        break;
    case TK_KW_WHILE:
        stmt = parse_while(self);
        break;
    case TK_KW_FOR:
        stmt = parse_for(self);
        break;
    default:
        start_position = location_clone(self->current->location);
        if (parser_check(self, TK_IDENT) || parser_check(self, TK_LPAREN)) {
            type_drop(parse_type(self));
            expr_drop(parse_primary_declaration_expr(self));
            if (parser_checks(self, 2, TK_EQ, TK_SEMICOLON)) {
                lexer_goto_location(self->lexer, start_position);
                token_drop(parser_advence(self));
                stmt = parse_var(self, false);
            }
        }

        if (stmt == NULL) {
            context_forget_errors(
                self->context,
                self->context->errors->len - errors_len);
            lexer_goto_location(self->lexer, start_position);
            token_drop(parser_advence(self));

            value.expr = parse_expr(self);
            stmt = new_stmt(STMT_EXPR, value);
        }
        parser_eat(self, TK_SEMICOLON);
        break;
    }
    return stmt;
}

void parser_drop(parser_t* self) {
    lexer_drop(self->lexer);
    token_drop(self->current);
    free(self);
}
