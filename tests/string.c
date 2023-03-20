#include <criterion/criterion.h>
#include "../src/string_utils.h"

Test(string, alloc_new_string) {
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");
    cr_assert_eq(str->len, 5);

    string_drop(str);
}

Test(string, get_string_char_at_index) {
    string_t* str = new_string("Hello");

    cr_assert_eq(string_get(str, 2), 'l');

    string_drop(str);
}

Test(string, get_string_slice) {
    string_t* str = new_string("Hello");
    string_t* slice = string_slice(str, 1, 4);

    cr_assert_str_eq(slice->data, "ell");
    cr_assert_eq(slice->len, 3);

    string_drop(str);
    string_drop(slice);
}

Test(string, push_a_char_ptr_into_a_string) {
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");

    string_push(str, " World");

    cr_assert_str_eq(str->data, "Hello World");
    cr_assert_eq(str->len, 11);

    string_drop(str);
}

Test(string, push_a_char_into_a_string) {
    string_t* str = new_string("Hello");

    cr_assert_str_eq(str->data, "Hello");

    string_push_char(str, '!');

    cr_assert_str_eq(str->data, "Hello!");
    cr_assert_eq(str->len, 6);

    string_drop(str);
}

Test(string, push_a_string_into_another_string) {
    string_t* str = new_string("Hello");
    string_t* str2 = new_string(" World");

    cr_assert_str_eq(str->data, "Hello");

    cr_assert_str_eq(str2->data, " World");
    cr_assert_eq(str2->len, 6);

    string_push_str(str, str2);

    cr_assert_str_eq(str->data, "Hello World");
    cr_assert_eq(str->len, 11);

    cr_assert_str_eq(str2->data, " World");
    cr_assert_eq(str2->len, 6);

    string_drop(str);
}