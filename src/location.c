#include <stdlib.h>
#include "location.h"

location_t* new_location(size_t line, size_t col, size_t start, size_t end) {
    location_t* location = malloc(sizeof(location_t));
    location->line = line;
    location->col = col;
    location->start = start;
    location->end = end;
    return location;
}

void location_drop(location_t* self) {
    free(self);
}
