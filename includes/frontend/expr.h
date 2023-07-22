#ifndef FRONTEND_EXPR_H
#define FRONTEND_EXPR_H
#include "../string_utils.h"
#include "token.h"
#include "type.h"

typedef struct Type type_t;
typedef struct Expr expr_t;

typedef struct BinaryExpr {
    expr_t* left;
    expr_t* right;
} binary_expr_t;

typedef struct UnaryExpr {
    expr_t* value;
} unary_expr_t;

typedef enum ExprKind {
    EX_ASSIGN,
    EX_ASSIGN_BIT_SL,    // <<=
    EX_ASSIGN_BIT_SR,    // >>=
    EX_ASSIGN_BOOL_AND,  // &&=
    EX_ASSIGN_BOOL_OR,   // ||=
    EX_ASSIGN_PLUS,      // +=
    EX_ASSIGN_MINUS,     // -=
    EX_ASSIGN_STAR,      // *=
    EX_ASSIGN_SLASH,     // /=
    EX_ASSIGN_PERCENT,   // %=
    EX_ASSIGN_BIT_AND,   // &=
    EX_ASSIGN_BIT_OR,    // |=
    EX_ASSIGN_BIT_XOR,   // ^=

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

    EX_ARRAY,
    EX_TUPLE,
    EX_IDENT,
    EX_INT_LITERAL,
    EX_FLOAT_LITERAL,
    EX_STRING_LITERAL,

    EX_NONE
} expr_kind_t;

typedef union ExprValue {
    // EX_BIN_*, EX_ASSIGN_*, EX_ASSIGN
    binary_expr_t* binary;

    // EX_UNA_*
    unary_expr_t* unary;

    // EX_ARRAY, EX_TUPLE
    /*expr_t*/vector_t* list;

    // EX_IDENT, EX_*_LITERAL,
    string_t* value;
} expr_value_t;

typedef struct ArgExpr {
    type_t* type;
    string_t* name;
} arg_expr_t;

typedef struct Expr {
    expr_kind_t kind;
    expr_value_t value;
} expr_t;

unary_expr_t* new_unary_expr(expr_t* value);
void unary_expr_drop(unary_expr_t* self);

arg_expr_t* new_arg_expr(type_t* type, string_t* name);
void arg_expr_drop(arg_expr_t* self);

binary_expr_t* new_binary_expr(expr_t* left, expr_t* right);
void binary_expr_drop(binary_expr_t* self);

expr_t* new_expr(expr_kind_t kind, expr_value_t value);
void expr_drop(expr_t* self);

expr_kind_t from_token(token_kind_t kind);
#endif
