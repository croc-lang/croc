#include <stdlib.h>
#include <frontend/token.h>

token_t* new_token(token_kind_t kind, string_t* value, location_t* location) {
    token_t* t = malloc(sizeof(token_t));
    t->kind = kind;
    t->value = value;
    t->location = location;
    return t;
}

string_t* token_get_value(token_t* self, char* or_default) {
    if (self->value == NULL) return new_string(or_default);
    return string_clone(self->value);
}

bool token_check(token_t* self, token_t* other) {
    return self->kind == other->kind && string_eq(self->value, other->value);
}

void token_drop(token_t* self) {
    location_drop(self->location);
    if (self->value)
        string_drop(self->value);
    free(self);
}
