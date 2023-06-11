#ifndef FRONTEND_STATEMENT_H
#define FRONTEND_STATEMENT_H
#include <stdbool.h>
#include "expr.h"
#include "type.h"
#include "../string_utils.h"
#include "../vector.h"

typedef struct Stmt stmt_t;
typedef struct IfStmt if_stmt_t;

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

typedef enum StmtKind {
    STMT_IMPORTS,
    STMT_MODULE,
    STMT_VAR_DECLARATION,
    STMT_FUNC_DEFINITION,
    STMT_IF,
    STMT_EXPR,
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
    // STMT_IF
    if_stmt_t* if_stmt;
    // STMT_EXPR
    expr_t* expr;
} stmt_value_t;

typedef struct Stmt {
    stmt_kind_t kind;
    stmt_value_t value;
} stmt_t;

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

if_stmt_t* new_if_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body,
    else_branch_stmt_t* else_branch
);
void if_stmt_drop(if_stmt_t* self);

stmt_t* new_stmt(stmt_kind_t kind, stmt_value_t value);
void stmt_drop(stmt_t* self);
#endif
