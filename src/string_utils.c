#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

/*
 * create a new string_t object with the string 'src'
 * return NULL if error occured.
 */
string_t* new_string(char* src) {
    size_t len = strlen(src);
    char* data = NULL;
    string_t* str = NULL;

    str = calloc(1, sizeof(string_t));

    if (str == NULL)
        return NULL;

    data = calloc(len + 1, sizeof(char));

    if (data == NULL)
        return NULL;

    memcpy(data, src, len);

    str->len = len;
    str->data = data;

    return str;
}

/*
 * create a new string_t object empty with a specific size of 'len'
 * return NULL if error occured.
 */
string_t* sized_string(size_t len) {
    char* data = NULL;
    string_t* str = NULL;

    str = calloc(1, sizeof(string_t));

    if (str == NULL)
        return NULL;

    data = calloc(len + 1, sizeof(char));

    if (data == NULL)
        return NULL;

    str->len = len;
    str->data = data;

    return str;
}

/*
 * resize string_t object 'self' with the new size 'len'.
 * return the new memory adresse with the new size allocated or NULL if realloc
 * failed.
 */
inline static char* string_resize(string_t* self, size_t len) {
    char* data = realloc(self->data, sizeof(char) * len + 1);

    if (data) {
        self->data = data;
        self->len = len;
    }

    return data;
}

/*
 * get the char at index 'index' in the string_t object 'self'.
 * if index is greater than or equal to size return -1 
 */
inline char string_get(string_t* self, size_t index) {
    if (index >= self->len)
        return -1;

    return self->data[index * sizeof(char)];
}

/**
 * compare the content of self with the str
 */
bool string_eq(string_t* self, char* str) {
    return strcmp(self->data, str) == 0;
}

/*
 * return new string_t object with the slice desired or NULL if an error
 * occured.
 */
string_t* string_slice(string_t* self, size_t start, size_t end) {
    char* str_sliced = strslice(self->data, start, end);
    string_t* str = new_string(str_sliced);

    if (str == NULL)
        return NULL;

    free(str_sliced);

    return str;
}

/*
 * push null terminated string to a string_t object.
 * return string_t object or NULL if an error occured
 */
string_t* string_push(string_t* self, char* str) {
    size_t new_size = self->len + strlen(str);

    if (string_resize(self, new_size)) {
        strncat(self->data, str, new_size);
        return self;
    }

    return NULL;
}

/*
 * push a char into a string_t object.
 * return string_t or NULL if an error occured.
 */
string_t* string_push_char(string_t* self, char c) {
    size_t new_size = self->len + 1;

    if (string_resize(self, new_size)) {
        self->data[self->len - 1] = c;
        return self;
    }

    return NULL;
}

/*
 * push string_t object 'str' inside the string_t object 'self'
 * return string_t or NULL if an error occured.
 */
string_t* string_push_str(string_t* self, string_t* str) {
    size_t new_size = self->len + str->len;

    if (string_resize(self, new_size)) {
        strncat(self->data, str->data, new_size);
        return self;
    }

    return NULL;
}

/*
 * free string_t object.
 */
void string_drop(string_t* self) {
    free(self->data);
    free(self);
}

/*
 * return new string with slice desired or NULL if error occured.
 */
char* strslice(const char *str, size_t start, size_t end) {
    char* result = calloc(sizeof(char), end - start + 1);

    if (result == NULL)
        return NULL;

    strncpy(result, str + start, end - start);

    return result;
}
