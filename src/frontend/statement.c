#include <stdlib.h>
#include "statement.h"

var_stmt_t* new_var_stmt(
    bool constant,
    stmt_t* left,
    stmt_t* right
) {
    var_stmt_t* stmt = malloc(sizeof(var_stmt_t));
    stmt->constant = constant;
    stmt->left = left;
    stmt->right = right;
    return stmt;
}

void var_stmt_drop(var_stmt_t* self) {
    stmt_drop(self->left);
    stmt_drop(self->right);
    free(self);
}

func_stmt_t* new_func_stmt(
    string_t* name,
    expr_t* return_type,
    /*expr_arg_t*/vector_t* args,
    /*stmt_t*/vector_t* body
) {
    func_stmt_t* stmt = malloc(sizeof(func_stmt_t));
    stmt->name = name;
    stmt->return_type = return_type;
    stmt->args = args;
    stmt->body = body;
    return stmt;
}

void func_stmt_drop(func_stmt_t* self) {
    string_drop(self->name);
    expr_drop(self->return_type);
    vector_drop(self->args);
    vector_drop(self->body);
    free(self);
}

if_stmt_t* new_if_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body
) {
    if_stmt_t* stmt = malloc(sizeof(if_stmt_t));
    stmt->condition = condition;
    stmt->body = body;
    return stmt;
}

void if_stmt_drop(if_stmt_t* self) {
    expr_drop(self->condition);
    vector_drop(self->body);
    free(self);
}

stmt_t* new_stmt(stmt_kind_t kind, stmt_value_t value) {
    stmt_t* stmt = malloc(sizeof(stmt_t));
    stmt->kind = kind;
    stmt->value = value;
    return stmt;
}

void stmt_drop(stmt_t* self) {
    free(self);
}
