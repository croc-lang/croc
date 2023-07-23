#include <stdlib.h>
#include <frontend/statement.h>

struct_property_t* new_struct_property(
    bool public,
    type_t* type,
    string_t* name
) {
    struct_property_t* property = malloc(sizeof(struct_property_t));
    property->public = public;
    property->type = type;
    property->name = name;
    return property;
}

void struct_property_drop(struct_property_t* self) {
    string_drop(self->name);
    type_drop(self->type);
    free(self);
}

import_stmt_t* new_import_stmt(
    string_t* file_paths,
    string_t* move_to,
    /*string_t*/vector_t* imports
) {
    import_stmt_t* stmt = malloc(sizeof(import_stmt_t));
    stmt->file_paths = file_paths;
    stmt->move_to = move_to;
    stmt->imports = imports;
    return stmt;
}

void import_stmt_drop(import_stmt_t* self) {
    string_drop(self->file_paths);
    if (self->move_to != NULL) string_drop(self->move_to);
    if (self->imports != NULL)
        vector_deeply_drop(self->imports, (void*)string_drop);
    free(self);
}

module_stmt_t* new_module_stmt(vector_t* path) {
    module_stmt_t* stmt = malloc(sizeof(module_stmt_t));
    stmt->path = path;
    return stmt;
}

void module_stmt_drop(module_stmt_t* self) {
    vector_deeply_drop(self->path, (void*)string_drop);
    free(self);
}

var_stmt_t* new_var_stmt(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
) {
    var_stmt_t* stmt = malloc(sizeof(var_stmt_t));
    stmt->public = false;
    stmt->constant = constant;
    stmt->type = type;
    stmt->left = left;
    stmt->right = right;
    return stmt;
}

void var_stmt_drop(var_stmt_t* self) {
    if (self->type != NULL) type_drop(self->type);
    expr_drop(self->left);
    if (self->right != NULL) expr_drop(self->right);
    free(self);
}

func_stmt_t* new_func_stmt(
    string_t* name,
    type_t* return_type,
    /*arg_expr_t*/vector_t* args,
    /*stmt_t*/vector_t* body
) {
    func_stmt_t* stmt = malloc(sizeof(func_stmt_t));
    stmt->public = false;
    stmt->name = name;
    stmt->return_type = return_type;
    stmt->args = args;
    stmt->body = body;
    return stmt;
}

void func_stmt_drop(func_stmt_t* self) {
    string_drop(self->name);
    if (self->return_type != NULL)
        type_drop(self->return_type);
    vector_deeply_drop(self->args, (void*)arg_expr_drop);
    vector_deeply_drop(self->body, (void*)stmt_drop);
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
    if (self->body != NULL)
        vector_deeply_drop(self->body, (void*)stmt_drop);
    else
        if_stmt_drop(self->if_branch);
    free(self);
}

while_stmt_t* new_while_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body
) {
    while_stmt_t* stmt = malloc(sizeof(while_stmt_t));
    stmt->condition = condition;
    stmt->body = body;
    return stmt;
}

void while_stmt_drop(while_stmt_t* self) {
    vector_deeply_drop(self->body, (void*)stmt_drop);
    expr_drop(self->condition);
    free(self);
}

primary_for_t* new_primary_for(
    for_init_value_t init,
    for_init_kind_t init_kind,
    expr_t* condition,
    expr_t* updater
) {
    primary_for_t* stmt = malloc(sizeof(primary_for_t));
    stmt->init = init;
    stmt->init_kind = init_kind;
    stmt->condition = condition;
    stmt->updater = updater;
    return stmt;
}

void primary_for_drop(primary_for_t* self) {
    if (self->init_kind == FOR_INIT_DECLA) var_stmt_drop(self->init.decla);
    else if (self->init_kind == FOR_INIT_EXPR) expr_drop(self->init.expr);
    if (self->condition != NULL) expr_drop(self->condition);
    if (self->updater != NULL) expr_drop(self->updater);
    free(self);
}

each_for_t* new_each_for(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
) {
    each_for_t* stmt = malloc(sizeof(each_for_t));
    stmt->constant = constant;
    stmt->type = type;
    stmt->left = left;
    stmt->right = right;
    return stmt;
}

void each_for_drop(each_for_t* self) {
    if (self->type != NULL) type_drop(self->type);
    expr_drop(self->left);
    expr_drop(self->right);
    free(self);
}

for_stmt_t* new_for_stmt(
    for_value_t value,
    for_kind_t kind,
    /*stmt_t*/vector_t* body
) {
    for_stmt_t* stmt = malloc(sizeof(for_stmt_t));
    stmt->value = value;
    stmt->kind = kind;
    stmt->body = body;
    return stmt;
}

void for_stmt_drop(for_stmt_t* self) {
    if (self->kind == FK_PRIMARY) primary_for_drop(self->value.primary);
    else if (self->kind == FK_EACH) each_for_drop(self->value.each);
    vector_deeply_drop(self->body, (void*)stmt_drop);
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
    if (self->else_branch != NULL)
        else_branch_stmt_drop(self->else_branch);
    vector_deeply_drop(self->body, (void*)stmt_drop);
    expr_drop(self->condition);
    free(self);
}

struct_stmt_t* new_struct_stmt(
    string_t* name,
    /*struct_property_t*/vector_t* properties
) {
    struct_stmt_t* stmt = malloc(sizeof(struct_stmt_t));
    stmt->name = name;
    stmt->properties = properties;
    return stmt;
}

void struct_stmt_drop(struct_stmt_t* self) {
    string_drop(self->name);
    if (self->properties != NULL)
        vector_deeply_drop(self->properties, (void*)struct_property_drop);
    free(self);
}

stmt_t* new_stmt(stmt_kind_t kind, stmt_value_t value) {
    stmt_t* stmt = malloc(sizeof(stmt_t));
    stmt->kind = kind;
    stmt->value = value;
    return stmt;
}

void stmt_drop(stmt_t* self) {
    if (self->kind == STMT_MODULE)
        module_stmt_drop(self->value.module);
    if (self->kind == STMT_IMPORTS)
        vector_deeply_drop(self->value.imports, (void*)import_stmt_drop);
    if (self->kind == STMT_VAR_DECLARATION)
        var_stmt_drop(self->value.var);
    else if (self->kind == STMT_FUNC_DEFINITION)
        func_stmt_drop(self->value.func);
    else if (self->kind == STMT_IF)
        if_stmt_drop(self->value.if_stmt);
    else if (self->kind == STMT_WHILE)
        while_stmt_drop(self->value.while_stmt);
    else if (self->kind == STMT_FOR)
        for_stmt_drop(self->value.for_stmt);
    else if (self->kind == STMT_STRUCT)
        struct_stmt_drop(self->value.struct_stmt);
    else if (
        self->kind == STMT_EXPR ||
        (self->kind == STMT_RETURN && self->value.expr != NULL))
            expr_drop(self->value.expr);
    free(self);
}
