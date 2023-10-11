#ifndef MEMORY_H
#define MEMORY_H
#include <vector.h>
#include <stdlib.h>

typedef vector_t memory_container_t;

memory_container_t* new_memory_container();
void memory_container_drop(memory_container_t* memory_container);

void* mem_alloc(size_t size);
void* mem_zeroed_alloc(size_t size);
void* mem_realloc(void* oldptr, size_t new_size);
void mem_free(void* ptr);
#endif