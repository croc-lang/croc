#include <memory.h>
#include <frontend/expr.h>

unary_expr_t* new_unary_expr(expr_t* value) {
    unary_expr_t* unary = mem_alloc(sizeof(unary_expr_t));
    unary->value = value;
    return unary;
}

void unary_expr_drop(unary_expr_t* self) {
    expr_drop(self->value);
    mem_free(self);
}

binary_expr_t* new_binary_expr(expr_t* left, expr_t* right) {
    binary_expr_t* binary = mem_alloc(sizeof(binary_expr_t));
    binary->left = left;
    binary->right = right;
    return binary;
}

void binary_expr_drop(binary_expr_t* self) {
    expr_drop(self->left);
    expr_drop(self->right);
    mem_free(self);
}

arg_expr_t* new_arg_expr(type_t* type, string_t* name) {
    arg_expr_t* arg = mem_alloc(sizeof(arg_expr_t));
    arg->type = type;
    arg->name = name;
    return arg;
}

void arg_expr_drop(arg_expr_t* self) {
    string_drop(self->name);
    type_drop(self->type);
    mem_free(self);
}

expr_t* new_expr(expr_kind_t kind, expr_value_t value) {
    expr_t* expr = mem_alloc(sizeof(expr_t));
    expr->kind = kind;
    expr->value = value;
    return expr;
}

void expr_drop(expr_t* self) {
    if (self->kind >= EX_ASSIGN && self->kind <= EX_BIN_MOD)
        binary_expr_drop(self->value.binary);
    else if (self->kind >= EX_UNA_SUFFIX_INCR && self->kind <= EX_UNA_NEG)
        unary_expr_drop(self->value.unary);
    else if (self->kind >= EX_ARRAY && self->kind <= EX_TUPLE)
        vector_deeply_drop(self->value.list, (void*)expr_drop);
    else if (self->kind >= EX_IDENT && self->kind <= EX_STRING_LITERAL)
        string_drop(self->value.value);
    mem_free(self);
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

    case TK_EQ: return EX_ASSIGN;
    case TK_ASG_BIT_SL: return EX_ASSIGN_BIT_SL;
    case TK_ASG_BIT_SR: return EX_ASSIGN_BIT_SR;
    case TK_ASG_BOOL_AND: return EX_ASSIGN_BOOL_AND;
    case TK_ASG_BOOL_OR: return EX_ASSIGN_BOOL_OR;
    case TK_ASG_PLUS: return EX_ASSIGN_PLUS;
    case TK_ASG_MINUS: return EX_ASSIGN_MINUS;
    case TK_ASG_STAR: return EX_ASSIGN_STAR;
    case TK_ASG_SLASH: return EX_ASSIGN_SLASH;
    case TK_ASG_PERCENT: return EX_ASSIGN_PERCENT;
    case TK_ASG_BIT_AND: return EX_ASSIGN_BIT_AND;
    case TK_ASG_BIT_OR: return EX_ASSIGN_BIT_OR;
    case TK_ASG_BIT_XOR: return EX_ASSIGN_BIT_XOR;

    default: return EX_NONE;
    }
}
