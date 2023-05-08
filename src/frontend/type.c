#include <stdlib.h>
#include "type.h"

path_type_t* new_path_type(/*string_t*/vector_t* segments) {
    path_type_t* path = malloc(sizeof(path_type_t));
    path->segments = segments;
    return path;
}

void path_type_drop(path_type_t* self) {
    vector_deeply_drop(self->segments, string_drop);
    free(self);
}

generic_type_t* new_generic_type(
    path_type_t* path,
    /*type_t*/vector_t* generics
) {
    generic_type_t* generic = malloc(sizeof(generic_type_t));
    generic->path = path;
    generic->generics = generics;
    return generic;
}

void generic_type_drop(generic_type_t* self) {
    path_type_drop(self->path);
    vector_deeply_drop(self->generics, type_drop);
    free(self);
}

array_type_t* new_array_type(path_type_t* path, expr_t* size) {
    array_type_t* array = malloc(sizeof(array_type_t));
    array->path = path;
    array->size = size;
    return array;
}

void array_type_drop(array_type_t* self) {
    path_type_drop(self->path);
    expr_drop(self->size);
    free(self);
}

type_t* new_type(type_kind_t kind, type_value_t value) {
    type_t* type = malloc(sizeof(type_t));
    type->kind = kind;
    type->value = value;
    return type;
}

void type_drop(type_t* self) {
    if (self->kind == TY_GENERIC)
        generic_type_drop(self->value.generic);
    else if (self->kind >= TY_POINTER && self->kind <= TY_PATH)
        path_type_drop(self->value.path);
    else if (self->kind == TY_ARRAY)
        array_type_drop(self->value.array);
    free(self);
}
