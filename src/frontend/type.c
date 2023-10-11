#include <memory.h>
#include <frontend/type.h>

generic_type_t* new_generic_type(
    /*string_t*/vector_t* path,
    /*type_t*/vector_t* generics
) {
    generic_type_t* generic = mem_alloc(sizeof(generic_type_t));
    generic->path = path;
    generic->generics = generics;
    return generic;
}

void generic_type_drop(generic_type_t* self) {
    vector_deeply_drop(self->path, (void*)string_drop);
    vector_deeply_drop(self->generics, (void*)type_drop);
    mem_free(self);
}

array_type_t* new_array_type(type_t* type, expr_t* size) {
    array_type_t* array = mem_alloc(sizeof(array_type_t));
    array->type = type;
    array->size = size;
    return array;
}

void array_type_drop(array_type_t* self) {
    type_drop(self->type);
    expr_drop(self->size);
    mem_free(self);
}

type_t* new_type(type_kind_t kind, type_value_t value) {
    type_t* type = mem_alloc(sizeof(type_t));
    type->kind = kind;
    type->value = value;
    return type;
}

void type_drop(type_t* self) {
    if (self->kind == TY_GENERIC)
        generic_type_drop(self->value.generic);
    else if (self->kind == TY_PATH)
        vector_deeply_drop(self->value.path, (void*)string_drop);
    else if (self->kind >= TY_POINTER && self->kind <= TY_SLICE)
        type_drop(self->value.type);
    else if (self->kind == TY_ARRAY)
        array_type_drop(self->value.array);
    mem_free(self);
}
