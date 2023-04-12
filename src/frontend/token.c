#include <stdlib.h>
#include "token.h"

token_t init_token(token_kind_t kind, string_t* value, location_t* location) {
    token_t t = {.kind = kind, .value = value, .location = location};
    return t;
}

void token_drop(token_t* self) {
    location_drop(self->location);
    free(self->value);
}
