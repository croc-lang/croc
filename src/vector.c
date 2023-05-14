#include <string.h>
#include <stdlib.h>
#include "vector.h"

vector_t* new_vector() {
    vector_t* vec = malloc(sizeof(vector_t));

    if (vec == NULL)
        return NULL;

    vec->capacity = 1;
    vec->len = 0;
    vec->data = NULL;

    return vec;
}

void* vector_get(vector_t* self, size_t index) {
    if (self->data == NULL || index >= self->len) return NULL;

    return self->data[index];
}

/*
 * resize vector_t object 'self' with the new size 'len'.
 * return the new memory adresse with the new size allocated or NULL if realloc
 * failed.
 */
inline static void** vector_resize(vector_t* self, size_t len) {
    void** data = NULL;

    if (self->capacity >= len && self->data != NULL) {
        self->len = len;
        return self->data;
    }

    size_t capacity = self->capacity * 2;
    if (self->data == NULL) data = calloc(capacity, sizeof(void*));
    else data = realloc(self->data, sizeof(void*) * capacity);

    if (data) {
        self->data = data;
        self->capacity = capacity;
        self->len = len;
    }

    return data;
}

void* vector_set(vector_t* self, size_t index, void* element) {
    if (index >= self->len) return NULL;

    void* last_value = vector_get(self, index);
    self->data[index] = element;
    return last_value;
}

vector_t* vector_push(vector_t* self, void* element) {
    if (vector_resize(self, self->len + 1)) {
        self->data[self->len - 1] = element;
        return self;
    }

    return NULL;
}

void* vector_pop(vector_t* self) {
    void* popped_element = vector_get(self, --self->len);
    self->data[self->len] = NULL;
    return popped_element;
}

vector_t* vector_clone(vector_t* self) {
    vector_t* vec = malloc(sizeof(vector_t));
    void** data = NULL;

    if (vec == NULL)
        return NULL;

    if (self->data) {
        data = malloc(self->len * sizeof(void*));
        memcpy(data, self->data, self->len * sizeof(void*));
    }

    vec->capacity = self->len;
    vec->len = self->len;
    vec->data = data;

    return vec;
}

void vector_deeply_drop(vector_t* self, void (*dropper)(void*)) {
    for (size_t i = 0; i < self->len; i++) {
        void* value = vector_get(self, i);
        dropper(value);
    }
    vector_drop(self);
}

void vector_drop(vector_t* self) {
    free(self->data);
    free(self);
}
