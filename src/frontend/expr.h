#ifndef FRONTEND_EXPR_H
#define FRONTEND_EXPR_H
#include <stdbool.h>
#include "../string_utils.h"

typedef struct BinaryExpr{
    expr_t* left;
    expr_t* right;
} binary_expr_t;

typedef struct UnaryExpr{
    expr_t* value;
} unary_expr_t;

typedef enum ExprKind {
    EX_VAR_ASSIGN,

    EX_BIN_ADD,
    EX_BIN_SUB,
    EX_BIN_MUL,
    EX_BIN_DIV,
    EX_BIN_MOD,

    EX_UNA_SUFFIX_INCR,
    EX_UNA_SUFFIX_DECR,
    EX_UNA_INCR,
    EX_UNA_DECR,
    EX_UNA_BINNOT,
    EX_UNA_NOT,
    EX_UNA_NEG,

    EX_IDENT,
    EX_INT_LETERAL,
    EX_FLOAT_LETERAL,
    EX_STRING_LITERAL,
} expr_kind_t;

typedef union ExprValue {
    // EX_BIN_*, EX_VAR_ASSIGN
    binary_expr_t* binary;

    // EX_UNA_*
    unary_expr_t* unary;

    // EX_IDENT, EX_*_LITERAL,
    string_t* value;
} expr_value_t;

typedef struct Expr {
    expr_kind_t kind;
    expr_value_t* value;
} expr_t;

unary_expr_t* new_unary_expr(expr_t* value);
void unary_expr_drop(unary_expr_t* self);

binary_expr_t* new_binary_expr(expr_t* left, expr_t* right);
void binary_expr_drop(binary_expr_t* self);

expr_t* new_expr(expr_kind_t kind, expr_value_t* value);
void expr_drop(expr_t* self);
#endif