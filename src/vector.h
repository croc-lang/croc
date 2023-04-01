#ifndef VECTOR_H
#define VECTOR_H
#include <stdio.h>
#include <stdbool.h>
typedef struct Vector {
    size_t capacity;
    size_t size;
    size_t len;
    void** data;
} vector_t;

vector_t* new_vector();
void* vector_get(vector_t* self, size_t index);

vector_t* vector_push(vector_t* self, void* element);
void vector_deeply_drop(vector_t* self, void (*dropper)(void*));
void vector_drop(vector_t* self);
#endif
