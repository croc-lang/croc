#ifndef FRONTEND_STATEMENT_H
#define FRONTEND_STATEMENT_H
#include <stdbool.h>
#include "expr.h"
#include "../string_utils.h"
#include "../vector.h"

typedef struct Stmt stmt_t;

typedef struct VarStmt{
    bool constant;
    stmt_t* left;
    stmt_t* right;
} var_stmt_t;

typedef struct FuncStmt{
    string_t* name;
    expr_t* return_type;
    /*expr_arg_t*/vector_t* args;
    /*stmt_t*/vector_t* body;
} func_stmt_t;

typedef struct IfStmt{
    expr_t* condition;
    /*stmt_t*/vector_t* body;
} if_stmt_t;

typedef enum StmtKind {
    STMT_VAR_DECLARATION,
    STMT_FUNC_DEFINITION,
    STMT_IF,
} stmt_kind_t;

typedef union StmtValue {
    // STMT_VAR_DECLARATION
    var_stmt_t* var;
    // STMT_FUNC_DEFINITION
    func_stmt_t* func;
    // STMT_IF
    if_stmt_t* if_stmt;
} stmt_value_t;

typedef struct Stmt {
    stmt_kind_t kind;
    stmt_value_t value;
} stmt_t;

var_stmt_t* new_var_stmt(
    bool constant,
    stmt_t* left,
    stmt_t* right
);
void var_stmt_drop(var_stmt_t* self);

func_stmt_t* new_func_stmt(
    string_t* name,
    expr_t* return_type,
    /*expr_arg_t*/vector_t* args,
    /*stmt_t*/vector_t* body
);
void func_stmt_drop(func_stmt_t* self);

if_stmt_t* new_if_stmt(
    expr_t* condition,
    /*stmt_t*/vector_t* body
);
void if_stmt_drop(if_stmt_t* self);

stmt_t* new_stmt(stmt_kind_t kind, stmt_value_t value);
void stmt_drop(stmt_t* self);
#endif