#include <criterion/criterion.h>
#include <string_utils.h>
#include <vector.h>
#include <memory.h>

Test(vector, alloc_new_vector) {
    memory_container_t* container = new_memory_container();
    vector_t* vector = new_vector();

    cr_assert_eq(vector->data, NULL);
    cr_assert_eq(vector->len, 0);
    cr_assert_eq(vector->capacity, 1);

    vector_drop(vector);
    memory_container_drop(container);
}

Test(vector, push_an_element_in_vector) {
    memory_container_t* container = new_memory_container();
    char* result[2] = {"a", "bc"};

    vector_t* vector = new_vector();
    vector_push(vector, "a");
    vector_push(vector, "bc");

    cr_assert_eq(vector->len, 2);
    cr_assert_eq(vector->capacity, 2);
    for (size_t i = 0; i < vector->len; i++) {
        char* actual = vector->data[i];
        char* expected = result[i];
        cr_assert_str_eq(actual, expected);
    }

    vector_drop(vector);
    memory_container_drop(container);
}

Test(vector, push_an_element_in_vector_and_get_it) {
    memory_container_t* container = new_memory_container();
    char* result[2] = {"a", "bc"};

    vector_t* vector = new_vector();
    vector_push(vector, "a");
    vector_push(vector, "bc");

    cr_assert_eq(vector->len, 2);
    cr_assert_eq(vector->capacity, 2);
    for (size_t i = 0; i < vector->len; i++) {
        char* actual = vector_get(vector, i);
        char* expected = result[i];
        cr_assert_str_eq(actual, expected);
    }
    char* actual = vector_get(vector, 3);
    cr_assert_null(actual);

    vector_drop(vector);
    memory_container_drop(container);
}

Test(vector, replace_element_in_vector) {
    memory_container_t* container = new_memory_container();
    int a = 0;
    int b = 1;
    int c = 2;

    int* result[2] = {&a, &c};
    vector_t* vector = new_vector();

    vector_push(vector, &a);
    vector_push(vector, &b);

    vector_replace(vector, &c, &b);
    for (size_t i = 0; i < vector->len; i++) {
        int* actual = vector_get(vector, i);
        int* expected = result[i];
        cr_assert_eq(actual, expected);
    }
    memory_container_drop(container);
}

Test(vector, clone) {
    memory_container_t* container = new_memory_container();
    vector_t* vector = new_vector();
    vector_push(vector, "a");
    vector_push(vector, "bc");
    vector_t* vector2 = vector_clone(vector);

    cr_assert_eq(vector->len, 2);
    cr_assert_eq(vector->capacity, 2);

    cr_assert_eq(vector2->len, 2);
    cr_assert_eq(vector2->capacity, 2);
    for (size_t i = 0; i < vector->len; i++) {
        char* actual = vector_get(vector, i);
        char* expected = vector_get(vector2, i);
        cr_assert_eq(actual, expected);
    }

    vector_drop(vector);
    vector_drop(vector2);
    memory_container_drop(container);
}

Test(vector, deep_freeing_of_a_vector) {
    memory_container_t* container = new_memory_container();
    cr_assert_eq(container->len, 0);
    string_t* str = new_string("Hello"); // 2 allocations
    string_t* str2 = new_string("World");

    vector_t* vector = new_vector(); // 1 allocation
    cr_assert_eq(container->len, 5);
    vector_push(vector, str); // 1 allocation
    vector_push(vector, str2); // 1 reallocation
    cr_assert_eq(container->len, 6);

    vector_deeply_drop(vector, (void*)string_drop);
    cr_assert_eq(container->len, 0);
    memory_container_drop(container);
}
