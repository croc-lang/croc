#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>

#define unreachable() { \
    fprintf(stderr, __FILE__":%d: unreachable code\n", __LINE__); \
    abort(); \
}
#endif
