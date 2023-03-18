#ifndef STRING_H
#define STRING_H
#include <stdio.h>
typedef struct String {
    size_t len;
    char* data;
} string_t;

string_t* new_string(char* str);
string_t* sized_string(size_t len);
char string_get(string_t* self, size_t index);
string_t* string_slice(string_t* self, size_t start, size_t end);
string_t* string_push(string_t* self, char* str2);
string_t* string_push_char(string_t* self, char c);
string_t* string_push_str(string_t* self, string_t* str2);
void string_drop(string_t* self);

char* strslice(const char *str, size_t start, size_t end);
#endif
