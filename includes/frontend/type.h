#ifndef FRONTEND_TYPE_H
#define FRONTEND_TYPE_H
#include "../string_utils.h"
#include "../vector.h"

typedef struct Type type_t;

typedef struct PathType {
    /*string_t*/vector_t* segments;
} path_type_t;

typedef struct GenericType {
    path_type_t* path;
    /*type_t*/vector_t* generics;
} generic_type_t;

// TODO(Hana): Work on the array type `example[2]`
// typedef struct ArrayType {
//     type_t* type;
//     expr_t* size;
// } array_type_t;

typedef enum TypeKind {
    TY_TUPLE,
    TY_GENERIC,
    // TY_ARRAY,
    TY_POINTER,
    TY_REFERENCE,
    TY_SLICE,
    TY_PATH,

    TY_NONE
} type_kind_t;

typedef union TypeValue {
    // TY_GENERIC
    generic_type_t* generic;

    // TY_PATH
    path_type_t* path;

    // TY_ARRAY
    // array_type_t* array;

    // TY_TUPLE
    vector_t* tuple;

    // TY_POINTER, TY_REFERENCE, TY_SLICE
    type_t* type;
} type_value_t;

typedef struct Type {
    type_kind_t kind;
    type_value_t value;
} type_t;

path_type_t* new_path_type(/*string_t*/vector_t* segments);
void path_type_drop(path_type_t* self);

generic_type_t* new_generic_type(
    path_type_t* path,
    /*type_t*/vector_t* generics
);
void generic_type_drop(generic_type_t* self);

// array_type_t* new_array_type(type_t* type, expr_t* size);
// void array_type_drop(array_type_t* self);

type_t* new_type(type_kind_t kind, type_value_t value);
void type_drop(type_t* self);
#endif
