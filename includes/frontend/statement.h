#ifndef FRONTEND_STATEMENT_H
#define FRONTEND_STATEMENT_H
#include <stdbool.h>
#include "expr.h"
#include "type.h"
#include "../string_utils.h"
#include "../vector.h"

typedef struct Stmt stmt_t;
typedef struct IfStmt if_stmt_t;

typedef struct StructStmt {
    string_t* name;
    /*struct_property_t*/vector_t* properties;
} struct_stmt_t;

typedef struct StructProperty {
    bool public;
    type_t* type;
    string_t* name;
} struct_property_t;

typedef struct ImportStmt {
    string_t* file_paths;
    // move the content of the file into a specific module, if NULL, no move
    string_t* move_to;
    /*string_t*/vector_t* imports;
} import_stmt_t;

typedef struct ModuleStmt {
    path_type_t* path;
} module_stmt_t;

typedef struct VarStmt {
    bool public;
    bool constant;
    // if the type is NULL, the type is infer
    type_t* type;
    expr_t* left;
    expr_t* right;
} var_stmt_t;

typedef struct FuncStmt {
    bool public;
    string_t* name;
    type_t* return_type;
    /*arg_expr_t*/vector_t* args;
    /*stmt_t*/vector_t* body;
} func_stmt_t;

typedef struct ElseBranchStmt {
    /*stmt_t*/vector_t* body;
    if_stmt_t* if_branch;
} else_branch_stmt_t;

typedef struct IfStmt {
    expr_t* condition;
    /*stmt_t*/vector_t* body;
    // NULL == no else branch
    else_branch_stmt_t* else_branch;
} if_stmt_t;

typedef struct WhileStmt {
    expr_t* condition;
    /*stmt_t*/vector_t* body;
} while_stmt_t;

typedef union ForInitValue {
    var_stmt_t* decla;
    expr_t* expr;
} for_init_value_t;

typedef enum ForInitKind {
    FOR_INIT_DECLA,
    FOR_INIT_EXPR,
    FOR_INIT_NONE,
} for_init_kind_t;

typedef struct EachFor {
    bool constant;
    // if the type is NULL, the type is infer
    type_t* type;
    expr_t* left;
    expr_t* right;
} each_for_t;

typedef struct PrimaryFor {
    for_init_value_t init;
    for_init_kind_t init_kind;
    expr_t* condition;
    expr_t* updater;
} primary_for_t;

typedef union ForValue {
    each_for_t* each;
    primary_for_t* primary;
} for_value_t;

typedef enum ForKind {
    FK_EACH,
    FK_PRIMARY,
    FK_NONE,
} for_kind_t;

typedef struct ForStmt {
    for_value_t value;
    for_kind_t kind;
    /*stmt_t*/vector_t* body;
} for_stmt_t;

typedef enum StmtKind {
    STMT_IMPORTS,
    STMT_MODULE,
    STMT_VAR_DECLARATION,
    STMT_FUNC_DEFINITION,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_STRUCT,
    STMT_EXPR,
    STMT_RETURN
} stmt_kind_t;

typedef union StmtValue {
    // STMT_IMPORTS
    /*import_stmt_t*/vector_t* imports;
    // STMT_MODULE
    module_stmt_t* module;
    // STMT_VAR_DECLARATION
    var_stmt_t* var;
    // STMT_FUNC_DEFINITION
    func_stmt_t* func;
    // STMT_WHILE
    while_stmt_t* while_stmt;
    // STMT_FOR
    for_stmt_t* for_stmt;
    // STMT_IF
    if_stmt_t* if_stmt;
    // STMT_STRUCT
    struct_stmt_t* struct_stmt;
    // STMT_EXPR, STMT_RETURN
    expr_t* expr;
} stmt_value_t;

typedef struct Stmt {
    stmt_kind_t kind;
    stmt_value_t value;
} stmt_t;

struct_property_t* new_struct_property(
    bool public,
    type_t* type,
    string_t* name
);
void struct_property_drop(struct_property_t* self);

import_stmt_t* new_import_stmt(
    string_t* file_paths,
    string_t* move_to,
    /*string_t*/vector_t* imports
);
void import_stmt_drop(import_stmt_t* self);

module_stmt_t* new_module_stmt(path_type_t* path);
void module_stmt_drop(module_stmt_t* self);

var_stmt_t* new_var_stmt(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
);
void var_stmt_drop(var_stmt_t* self);

func_stmt_t* new_func_stmt(
    string_t* name,
    type_t* return_type,
    /*arg_expr_t*/vector_t* args,
    /*stmt_t*/vector_t* body
);
void func_stmt_drop(func_stmt_t* self);

else_branch_stmt_t* new_else_branch_stmt(
    /*stmt_t*/vector_t* body,
    if_stmt_t* if_branch
);
void else_branch_stmt_drop(else_branch_stmt_t* self);

while_stmt_t* new_while_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body
);
void while_stmt_drop(while_stmt_t* self);

primary_for_t* new_primary_for(
    for_init_value_t init,
    for_init_kind_t init_kind,
    expr_t* condition,
    expr_t* updater
);
void primary_for_drop(primary_for_t* self);

each_for_t* new_each_for(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
);
void each_for_drop(each_for_t* self);

for_stmt_t* new_for_stmt(
    for_value_t value,
    for_kind_t kind,
    /*stmt_t*/vector_t* body
);
void for_stmt_drop(for_stmt_t* self);

if_stmt_t* new_if_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body,
    else_branch_stmt_t* else_branch
);
void if_stmt_drop(if_stmt_t* self);

struct_stmt_t* new_struct_stmt(
    string_t* name,
    /*struct_property_t*/vector_t* properties
);
void struct_stmt_drop(struct_stmt_t* self);

stmt_t* new_stmt(stmt_kind_t kind, stmt_value_t value);
void stmt_drop(stmt_t* self);
#endif
