#ifndef LOCATION_H
#define LOCATION_H
#include <stdio.h>
typedef struct Location {
    size_t line;
    size_t col;
    size_t start;
    size_t end;
} location_t;

location_t* new_location(size_t line, size_t col, size_t pos, size_t size);
location_t* location_clone(location_t* self);
void location_drop(location_t* self);
#endif
