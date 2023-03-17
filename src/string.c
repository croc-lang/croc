#include <stdlib.h>
#include <string.h>
#include "string.h"

string_t* new_string(char* str) {
    size_t len = strlen(str);
    string_t* string = calloc(1, sizeof(string_t));
    char* data = calloc(len + 1, sizeof(char));
    memcpy(data, str, len);
    
    string->len = len;
    string->data = data;
    return string;
}

inline static void string_resize(string_t* self, size_t len) {
    self->len = len;
    char* data = realloc(self->data, sizeof(char) * len + 1);
    if (data == NULL) {
        return;
    }
    self->data = data;
}

inline char string_get(string_t* self, size_t index) {
    return *(self->data + (index * sizeof(char)));
}


string_t* string_slice(string_t* self, size_t start, size_t end) {
    char* str_sliced = strslice(self->data, start, end);
    string_t* string = new_string(str_sliced);
    free(str_sliced);
    return string;
}

void string_push(string_t* self, char* str) {
    string_resize(self, self->len + strlen(str));
    strcat(self->data, str);
}

void string_push_str(string_t* self, string_t* str) {
    string_resize(self, self->len + str->len);
    strcat(self->data, str->data);
}

void string_drop(string_t* self) {
    free(self->data);
    free(self);
}

char* strslice(const char *str, size_t start, size_t end) {
    char* result = calloc(sizeof(char), end - start + 1);
    strncpy(result, str + start, end - start);
    return result;
}
