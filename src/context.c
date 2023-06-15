#include <stdlib.h>
#include <context.h>

ctx_error_t* new_error(
    ctx_error_kind_t kind,
    string_t* msg,
    location_t* location
) {
    ctx_error_t* error = malloc(sizeof(ctx_error_t));
    error->kind = kind;
    error->msg = msg;
    error->location = location;
    return error;
}

void error_drop(ctx_error_t* self) {
    location_drop(self->location);
    string_drop(self->msg);
    free(self);
}

context_t* new_context(void) {
    context_t* context = malloc(sizeof(context_t));
    context->errors = new_vector();
    return context;
}

bool context_add_error(context_t* self, ctx_error_t* error) {
    return vector_push(self->errors, error) != NULL;
}

void context_forget_errors(context_t* self, int number) {
    for (int i = 0; i < number; i++) {
        vector_pop(self->errors);
    }
}

void context_drop(context_t* self) {
    vector_deeply_drop(self->errors, (void*)error_drop);
    free(self);
}
