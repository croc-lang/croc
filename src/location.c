#include <memory.h>
#include <location.h>

location_t* new_location(size_t line, size_t col, size_t start, size_t end) {
    location_t* location = mem_alloc(sizeof(location_t));
    location->line = line;
    location->col = col;
    location->start = start;
    location->end = end;
    return location;
}

location_t* location_clone(location_t* self) {
    return new_location(self->line, self->col, self->start, self->end);
}

void location_drop(location_t* self) {
    mem_free(self);
}
