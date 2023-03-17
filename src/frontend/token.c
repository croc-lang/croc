#include <stdlib.h>
#include "token.h"

token_t init_token(token_kind_t kind, string_t* value) {
    token_t t = {.kind = kind, .value = value};
    return t;
}

void token_drop(token_t* self) {
    free(self->value);
}
