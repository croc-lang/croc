#include <stdlib.h>
#include "expr.h"

unary_expr_t* new_unary_expr(expr_t* value) {
    unary_expr_t* unary = malloc(sizeof(unary_expr_t));
    unary->value = value;
    return unary;
}

void unary_expr_drop(unary_expr_t* self) {
    expr_drop(self->value);
    free(self);
}

binary_expr_t* new_binary_expr(expr_t* left, expr_t* right) {
    binary_expr_t* binary = malloc(sizeof(binary_expr_t));
    binary->left = left;
    binary->right = right;
    return binary;
}

void binary_expr_drop(binary_expr_t* self) {
    expr_drop(self->left);
    expr_drop(self->right);
    free(self);
}

arg_expr_t* new_arg_expr(type_t* type, string_t* name) {
    arg_expr_t* arg = malloc(sizeof(arg_expr_t));
    arg->type = type;
    arg->name = name;
    return arg;
}

void arg_expr_drop(arg_expr_t* self) {
    string_drop(self->name);
    type_drop(self->type);
    free(self);
}

expr_t* new_expr(expr_kind_t kind, expr_value_t value) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = kind;
    expr->value = value;
    return expr;
}

void expr_drop(expr_t* self) {
    if (self->kind >= EX_VAR_ASSIGN && self->kind <= EX_BIN_MOD)
        binary_expr_drop(self->value.binary);
    else if (self->kind >= EX_UNA_SUFFIX_INCR && self->kind <= EX_UNA_NEG)
        unary_expr_drop(self->value.unary);
    else if (self->kind >= EX_VAR_ASSIGN && self->kind <= EX_BIN_MOD)
        string_drop(self->value.value);
    free(self);
}

expr_kind_t from_token(token_kind_t kind) {
    switch (kind) {
    case TK_PLUS: return EX_BIN_ADD;
    case TK_MINUS: return EX_BIN_SUB;
    case TK_STAR: return EX_BIN_MUL;
    case TK_SLASH: return EX_BIN_DIV;
    case TK_PERCENT: return EX_BIN_MOD;

    case TK_IDENT: return EX_IDENT;
    case TK_STRING: return EX_STRING_LITERAL;
    case TK_INT: return EX_INT_LITERAL;
    case TK_FLOAT: return EX_FLOAT_LITERAL;

    default: return EX_NONE;
    }
}
