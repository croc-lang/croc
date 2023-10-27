#include <string.h>
#include <memory.h>
#include <utils.h>

memory_container_t* container = NULL;

inline static void** memory_container_resize(
    memory_container_t* self,
    size_t len
) {
    void** data = NULL;

    if (self->capacity >= len && self->data != NULL) {
        self->len = len;
        return self->data;
    }

    size_t capacity = self->capacity * 2;
    if (self->data == NULL) data = calloc(capacity, sizeof(void*));
    else data = realloc(self->data, sizeof(void*) * capacity);

    if (data == NULL)
        panic("Memory overload, unable to allocate more memory.");

    self->data = data;
    self->capacity = capacity;
    self->len = len;

    return data;
}

inline static memory_container_t* memory_container_push(
    memory_container_t* self,
    void* element
) {
    if (memory_container_resize(self, self->len + 1)) {
        self->data[self->len - 1] = element;
        return self;
    }

    return NULL;
}

memory_container_t* new_memory_container() {
    if (container != NULL) memory_container_drop(container);

    container = malloc(sizeof(vector_t));
    if (container == NULL)
        panic("Memory overload, unable to allocate more memory.");

    container->capacity = 1;
    container->len = 0;
    container->data = NULL;

    return container;
}

void memory_container_drop(memory_container_t* self) {
    self = self == NULL ? container : self;
    if (self == NULL) return;

    for (size_t i = 0; i < self->len; i++) {
        void* value = vector_get(self, i);
        free(value);
    }

    if (self == container)
        container = NULL;

    free(self->data);
    free(self);
}

void* mem_alloc(size_t size) {
    #ifdef DEBUG
        if (container == NULL)
            panic("The memory container needs to be initialized.");
    #endif

    void* ptr = malloc(size);
    if (ptr == NULL) panic("Memory overload, unable to allocate more memory.");

    memory_container_push(container, ptr);
    return ptr;
}

void* mem_zeroed_alloc(size_t size) {
    void* ptr = mem_alloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void* mem_realloc(void* oldptr, size_t new_size) {
    #ifdef DEBUG
        if (container == NULL)
            panic("The memory container needs to be initialized.");
    #endif

    void* ptr = realloc(oldptr, new_size);
    if (ptr == NULL)
        panic("Memory overload, unable to allocate more memory.");

    // no after free because in replace we dont touch the content of oldptr
    // but oldptr it-self
    if (ptr != oldptr) vector_replace(container, ptr, oldptr);
    return ptr;
}

void mem_free(void* ptr) {
    #ifdef DEBUG
        if (container == NULL)
            panic("The memory container needs to be initialized.");
    #endif

    if (vector_remove(container, ptr))
        free(ptr);
}
