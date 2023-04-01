#include <criterion/criterion.h>
#include "../src/string_utils.h"
#include "../src/vector.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
// code source from https://www.youtube.com/watch?v=yM9zteeTCiI
int is_mapped(const void* ptr, int bytes) {
    if (ptr == NULL) return 0;

    int valid = 1;
    // create pipe
    int fd[2];
    pipe(fd);
    
    int a = write(fd[1], ptr, bytes);
    if (a < 0) valid = 0;

    close(fd[0]);
    close(fd[1]);

    return valid;
}

Test(vector, alloc_new_vector) {
    vector_t* vector = new_vector();

    cr_assert_eq(vector->data, NULL);
    cr_assert_eq(vector->len, 0);
    cr_assert_eq(vector->capacity, 1);

    vector_drop(vector);
}

Test(vector, push_an_element_in_vector) {
    char* array[2] = {"a", "bc"};

    vector_t* vector = new_vector(sizeof(char*));
    vector_push(vector, "a");
    vector_push(vector, "bc");

    cr_assert_eq(vector->len, 2);
    cr_assert_eq(vector->capacity, 2);
    for (size_t i = 0; i < vector->len; i++) {
        char* actual = vector->data[i];
        char* expected = array[i];
        cr_assert_str_eq(actual, expected);
    }

    vector_drop(vector);
}

Test(vector, push_an_element_in_vector_and_get_it) {
    char* array[2] = {"a", "bc"};

    vector_t* vector = new_vector(sizeof(char*));
    vector_push(vector, "a");
    vector_push(vector, "bc");

    cr_assert_eq(vector->len, 2);
    cr_assert_eq(vector->capacity, 2);
    for (size_t i = 0; i < vector->len; i++) {
        char* actual = vector_get(vector, i);
        char* expected = array[i];
        cr_assert_str_eq(actual, expected);
    }
    char* actual = vector_get(vector, 3);
    cr_assert_null(actual);

    vector_drop(vector);
}

// TODO(hana): test ?
// Test(vector, deep_freeing_of_a_vector) {
//     string_t* str = new_string("Hello");
//     string_t* str2 = new_string("World");

//     vector_t* vector = new_vector(sizeof(string_t*));
//     vector_push(vector, str);
//     vector_push(vector, str2);

//     vector_deeply_drop(vector, string_drop);
// }
