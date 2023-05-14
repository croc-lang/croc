#include <stdlib.h>
#include "statement.h"

var_stmt_t* new_var_stmt(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
) {
    var_stmt_t* stmt = malloc(sizeof(var_stmt_t));
    stmt->constant = constant;
    stmt->type = type;
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

else_branch_stmt_t* new_else_branch_stmt(
    /*stmt_t*/vector_t* body,
    if_stmt_t* if_branch
) {
    else_branch_stmt_t* stmt = malloc(sizeof(else_branch_stmt_t));
    stmt->body = body;
    stmt->if_branch = if_branch;
    return stmt;
}

void else_branch_stmt_drop(else_branch_stmt_t* self) {
    vector_deeply_drop(self->body, stmt_drop);
    if_stmt_drop(self->if_branch);
    free(self);
}

if_stmt_t* new_if_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body,
    else_branch_stmt_t* else_branch
) {
    if_stmt_t* stmt = malloc(sizeof(if_stmt_t));
    stmt->condition = condition;
    stmt->body = body;
    stmt->else_branch = else_branch;
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
