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

expr_t* new_expr(expr_kind_t kind, expr_value_t* value) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = kind;
    expr->value = value;
    return expr;
}

void expr_drop(expr_t* self) {
    if (self->kind >= EX_VAR_ASSIGN && self->kind <= EX_BIN_MOD)
        binary_expr_drop(self->value);
    else if (self->kind >= EX_UNA_SUFFIX_INCR && self->kind <= EX_UNA_NEG)
        unary_expr_drop(self->value);
    else if (self->kind >= EX_VAR_ASSIGN && self->kind <= EX_BIN_MOD)
        string_drop(self->value);
    free(self);
}
