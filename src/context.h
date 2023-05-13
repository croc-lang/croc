#ifndef CONTEXT_H
#define CONTEXT_H
#include <stdio.h>
#include "string_utils.h"
#include "location.h"
#include "vector.h"
typedef enum ErrorKind {
    // lexer
    CTX_ERR_FILE_NOT_FOUND,
    CTX_ERR_UNTERMINATED_STR,
    CTX_ERR_INVALID_SLICE,
    CTX_ERR_INVALID_SYNTAX,
    CTX_ERR_INVALID_CHAR,

    // parser
    CTX_ERR_INVALID_TOKEN
} ctx_error_kind_t;

typedef struct Error {
    ctx_error_kind_t kind;
    location_t* location;
    string_t* msg;
} ctx_error_t;

typedef struct Context {
    /*ctx_error_t*/vector_t* errors;
    // vector_t* warnings;
} context_t;

ctx_error_t* new_error(
    ctx_error_kind_t kind,
    string_t* msg,
    location_t* location
);
void error_drop(ctx_error_t* self);

context_t* new_context();
bool context_add_error(context_t* self, ctx_error_t* error);
void context_forget_errors(context_t* self, int number);
void context_drop(context_t* self);
#endif
