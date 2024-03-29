#include <criterion/criterion.h>
#include <string_utils.h>
#include <memory.h>

Test(string, alloc_new_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");
    cr_assert_eq(str->len, 5);
    cr_assert_eq(str->capacity, 5);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, empty_sized_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = sized_string(10);

    cr_assert_str_eq(str->data, "");
    cr_assert_eq(str->len, 0);
    cr_assert_eq(str->capacity, 10);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, formating_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = format_string("Hi %s!", "Hana");

    cr_assert_str_eq(str->data, "Hi Hana!");
    cr_assert_eq(str->len, 8);
    cr_assert_eq(str->capacity, 8);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, get_string_char_at_index) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");

    cr_assert_eq(string_get(str, 2), 'l');

    string_drop(str);
    memory_container_drop(container);
}

Test(string, get_string_slice) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");
    string_t* slice = string_slice(str, 1, 4);

    cr_assert_str_eq(slice->data, "ell");
    cr_assert_eq(slice->len, 3);
    cr_assert_eq(slice->capacity, 3);

    string_drop(str);
    string_drop(slice);
    memory_container_drop(container);
}

Test(string, push_a_char_ptr_into_a_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");
    cr_assert_eq(str->capacity, 5);

    string_push(str, " World");

    cr_assert_str_eq(str->data, "Hello World");
    cr_assert_eq(str->len, 11);
    cr_assert_eq(str->capacity, 15);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, push_a_char_into_a_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");

    string_push_char(str, '!');

    cr_assert_str_eq(str->data, "Hello!");
    cr_assert_eq(str->len, 6);
    cr_assert_eq(str->capacity, 10);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, push_a_string_into_another_string) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");
    string_t* str2 = new_string(" World");

    cr_assert_str_eq(str->data, "Hello");

    cr_assert_str_eq(str2->data, " World");
    cr_assert_eq(str2->len, 6);

    string_push_str(str, str2);

    cr_assert_str_eq(str->data, "Hello World");
    cr_assert_eq(str->len, 11);
    cr_assert_eq(str->capacity, 15);

    cr_assert_str_eq(str2->data, " World");
    cr_assert_eq(str2->len, 6);

    string_drop(str);
    memory_container_drop(container);
}

Test(string, equality) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");
    string_t* str2 = new_string("Hello");

    cr_assert(string_eq(str, str2));
    cr_assert(string_eq_str(str, "Hello"));

    string_drop(str);
    memory_container_drop(container);
}

Test(string, start_with) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello World!");

    cr_assert(string_offset_start_with(str, "Hello", 0));
    cr_assert(string_offset_start_with(str, "World", 6));
    cr_assert_not(string_offset_start_with(str, "Hello", 6));

    string_drop(str);
    memory_container_drop(container);
}

Test(string, clone) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");
    string_t* str2 = string_clone(str);;

    cr_assert(string_eq(str, str2));
    cr_assert_neq(str, str2);

    string_drop(str);
    string_drop(str2);
    memory_container_drop(container);
}
