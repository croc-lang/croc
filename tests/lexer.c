#include <criterion/criterion.h>
#include <frontend/lexer.h>
#include <frontend/token.h>
#include <memory.h>

Test(lexer, normal_string) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "\"I'm a string\"");
    token_t* expected_token = new_token(
        TK_STRING,
        new_string("I'm a string"),
        NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, string_with_escapement) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "\"I'm a \\\"string\\\"\"");
    token_t* expected_token = new_token(
        TK_STRING,
        new_string("I'm a \"string\""),
        NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, string_with_special_char) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "\"I'm a \\nstring\"");
    token_t* expected_token = new_token(
        TK_STRING,
        new_string("I'm a \nstring"),
        NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, int) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "10");
    token_t* expected_token = new_token(TK_INT, new_string("10"), NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, float) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "10.4");
    token_t* expected_token = new_token(TK_FLOAT, new_string("10.4"), NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, ident) {
    memory_container_t* container = new_memory_container();
    lexer_t* lexer = new_lexer("test.cr", "identifier");
    token_t* expected_token = new_token(
        TK_IDENT,
        new_string("identifier"),
        NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, keywords) {
    memory_container_t* container = new_memory_container();
    const int size = 3;

    lexer_t* lexer = new_lexer("test.cr", "let if func");
    token_kind_t keywords[] = {TK_KW_LET, TK_KW_IF, TK_KW_FUNC};

    for (size_t i = 0; i < size; i++) {
        token_t* actual = lexer_next_token(lexer);

        cr_assert_null(actual->value);
        cr_assert_eq(actual->kind, keywords[i]);

        token_drop(actual);
    }

    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, operators) {
    memory_container_t* container = new_memory_container();
    const int size = 31;

    lexer_t* lexer = new_lexer("test.cr", "( ) { } [ ] , ; :: -> "
                                          "+ - * / % | & ^ ! ~ ++ -- "
                                          "= == != < > <= >= << >> || && "
                                          "<<= >>= &&= ||= "
                                          "+= -= *= /= %= &= |= ^=");

    token_kind_t keywords[] = {
        TK_LPAREN,
        TK_RPAREN,
        TK_LBRACE,
        TK_RBRACE,
        TK_LBRACKET,
        TK_RBRACKET,
        TK_COMMA,
        TK_SEMICOLON,
        TK_DB_COLON,
        TK_ARROW,
        TK_PLUS,
        TK_MINUS,
        TK_STAR,
        TK_SLASH,
        TK_PERCENT,
        TK_BIN_OR,
        TK_BIN_AND,
        TK_BIN_XOR,
        TK_BANG,
        TK_TILDE,
        TK_INCREMENT,
        TK_DECREMENT,
        TK_EQ,
        TK_CMP_EQ,
        TK_CMP_NE,
        TK_CMP_LT,
        TK_CMP_GT,
        TK_CMP_LE,
        TK_CMP_GE,
        TK_BIT_SL,
        TK_BIT_SR,
        TK_BOOL_OR,
        TK_BOOL_AND,
        TK_ASG_BIT_SL,
        TK_ASG_BIT_SR,
        TK_ASG_BOOL_AND,
        TK_ASG_BOOL_OR,
        TK_ASG_PLUS,
        TK_ASG_MINUS,
        TK_ASG_STAR,
        TK_ASG_SLASH,
        TK_ASG_PERCENT,
        TK_ASG_BIT_AND,
        TK_ASG_BIT_OR,
        TK_ASG_BIT_XOR,
    };

    for (size_t i = 0; i < size; i++) {
        token_t* actual = lexer_next_token(lexer);

        cr_assert_null(actual->value);
        cr_assert_eq(actual->kind, keywords[i]);

        token_drop(actual);
    }

    lexer_drop(lexer);
    memory_container_drop(container);
}

Test(lexer, token_proximity) {
    memory_container_t* container = new_memory_container();
    const int size = 3;

    lexer_t* lexer = new_lexer("test.cr", "test()");

    token_kind_t keywords[] = {
        TK_IDENT,
        TK_LPAREN,
        TK_RPAREN,
    };

    for (size_t i = 0; i < size; i++) {
        token_t* actual = lexer_next_token(lexer);

        cr_assert_eq(actual->kind, keywords[i]);
        if (keywords[i] == TK_IDENT)
            cr_assert_str_eq(actual->value->data, "test");
        else
            cr_assert_null(actual->value);

        token_drop(actual);
    }

    lexer_drop(lexer);
    memory_container_drop(container);
}
