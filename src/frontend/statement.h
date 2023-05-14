#ifndef FRONTEND_STATEMENT_H
#define FRONTEND_STATEMENT_H
#include <stdbool.h>
#include "expr.h"
#include "type.h"
#include "../string_utils.h"
#include "../vector.h"

typedef struct Stmt stmt_t;
typedef struct IfStmt if_stmt_t;

typedef struct VarStmt {
    bool constant;
    // if the type is NULL, the type is infer
    type_t* type;
    expr_t* left;
    expr_t* right;
} var_stmt_t;

typedef struct FuncStmt {
    string_t* name;
    expr_t* return_type;
    /*expr_arg_t*/vector_t* args;
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
    STMT_VAR_DECLARATION,
    STMT_FUNC_DEFINITION,
    STMT_IF,
    STMT_EXPR,
} stmt_kind_t;

typedef union StmtValue {
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

var_stmt_t* new_var_stmt(
    bool constant,
    type_t* type,
    expr_t* left,
    expr_t* right
);
void var_stmt_drop(var_stmt_t* self);

func_stmt_t* new_func_stmt(
    string_t* name,
    expr_t* return_type,
    /*expr_arg_t*/vector_t* args,
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
