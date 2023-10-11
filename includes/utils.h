#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define unreachable() panic(__FILE__":%d: unreachable code\n", __LINE__)
#define panic(...) { \
    fprintf(stderr, __VA_ARGS__); \
    memory_container_drop(NULL); \
    abort(); \
}
#endif
