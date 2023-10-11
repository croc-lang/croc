#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <string_utils.h>

/*
 * create a new string_t object with the string 'src'
 * return NULL if error occured.
 */
string_t* new_string(char* src) {
    size_t len = strlen(src);
    string_t* str = mem_alloc(sizeof(string_t));
    char* data = mem_zeroed_alloc(sizeof(char) * (len + 1));

    memcpy(data, src, len);

    str->capacity = len ? len : 1;
    str->len = len;
    str->data = data;

    return str;
}

/*
 * create a new string_t object empty with a specific size of 'len'
 * return NULL if error occured.
 */
string_t* sized_string(size_t len) {
    string_t* str = mem_alloc(sizeof(string_t));
    char* data = mem_zeroed_alloc(sizeof(char) * (len + 1));

    str->capacity = len ? len : 1;
    str->data = data;
    str->len = 0;

    return str;
}

string_t* format_string(const char* src, ...) {
    va_list args;

    va_start(args, src);
    int len = vsnprintf(NULL, 0, src, args);
    va_end(args);
    string_t* str = sized_string(len);
    str->len = len;

    va_start(args, src);
    vsnprintf(str->data, len + 1, src, args);
    va_end(args);
    return str;
}

/*
 * resize string_t object 'self' with the new size 'len'.
 * return the new memory adresse with the new size allocated or NULL if realloc
 * failed.
 */
inline static void string_resize(string_t* self, size_t len) {
    if (self->capacity >= len) {
        self->len = len;
        return;
    }

    size_t capacity = self->capacity * (len / self->capacity + 1);
    char* data = mem_realloc(self->data, sizeof(char) * capacity);

    self->data = data;
    self->capacity = capacity;
    self->len = len;
}

/*
 * get the char at index 'index' in the string_t object 'self'.
 * if index is greater than or equal to size return -1 
 */
inline char string_get(string_t* self, size_t index) {
    if (index >= self->len)
        return -1;

    return self->data[index];
}

/**
 * compare the content of self with the str
 */
bool string_eq_str(string_t* self, char* str) {
    return strcmp(self->data, str) == 0;
}

/**
 * compare the content of self with another string_t
 */
bool string_eq(string_t* self, string_t* str) {
    return strcmp(self->data, str->data) == 0;
}

bool string_offset_start_with(string_t* self, char* str, size_t offset) {
    return strncmp(self->data + offset, str, strlen(str)) == 0;
}

/*
 * return new string_t object with the slice desired or NULL if an error
 * occured.
 */
string_t* string_slice(string_t* self, size_t start, size_t end) {
    char* str_sliced = strslice(self->data, start, end);
    string_t* str = new_string(str_sliced);

    mem_free(str_sliced);
    return str;
}

/*
 * push null terminated string to a string_t object.
 * return string_t object or NULL if an error occured
 */
string_t* string_push(string_t* self, char* str) {
    size_t new_size = self->len + strlen(str);

    string_resize(self, new_size);
    strncat(self->data, str, new_size);
    return self;
}

/*
 * push a char into a string_t object.
 * return string_t or NULL if an error occured.
 */
string_t* string_push_char(string_t* self, char c) {
    size_t new_size = self->len + 1;

    string_resize(self, new_size);
    self->data[self->len - 1] = c;
    self->data[self->len] = '\0';
    return self;
}

/*
 * push string_t object 'str' inside the string_t object 'self'
 * return string_t or NULL if an error occured.
 */
string_t* string_push_str(string_t* self, string_t* str) {
    size_t new_size = self->len + str->len;

    string_resize(self, new_size);
    strncat(self->data, str->data, new_size);
    return self;
}

string_t* string_clone(string_t* self) {
    string_t* str = mem_alloc(sizeof(string_t));
    char* data = mem_alloc(self->len + 1);

    memcpy(data, self->data, self->len + 1);

    str->capacity = self->len;
    str->len = self->len;
    str->data = data;

    return str;
}

/*
 * free string_t object.
 */
void string_drop(string_t* self) {
    mem_free(self->data);
    mem_free(self);
}

/*
 * return new string with slice desired or NULL if error occured.
 */
char* strslice(const char *str, size_t start, size_t end) {
    char* result = mem_zeroed_alloc(sizeof(char) * end - start + 1);

    strncpy(result, str + start, end - start);

    return result;
}
