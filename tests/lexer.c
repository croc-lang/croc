#include <criterion/criterion.h>
#include "../src/frontend/lexer.h"
#include "../src/frontend/token.h"

Test(lexer, normal_string) {
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
}

Test(lexer, string_with_escapement) {
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
}

Test(lexer, string_with_special_char) {
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
}

Test(lexer, int) {
    lexer_t* lexer = new_lexer("test.cr", "10");
    token_t* expected_token = new_token(TK_INT, new_string("10"), NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
}

Test(lexer, float) {
    lexer_t* lexer = new_lexer("test.cr", "10.4");
    token_t* expected_token = new_token(TK_FLOAT, new_string("10.4"), NULL);
    token_t* actual = lexer_next_token(lexer);

    cr_expect(token_check(actual, expected_token));

    token_drop(expected_token);
    token_drop(actual);
    lexer_drop(lexer);
}

Test(lexer, ident) {
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
}

Test(lexer, keywords) {
    const int size = 3;

    lexer_t* lexer = new_lexer("test.cr", "let if func");
    token_kind_t keywords[size] = {TK_KW_LET, TK_KW_IF, TK_KW_FUNC};

    for (size_t i = 0; i < size; i++) {
        token_t* actual = lexer_next_token(lexer);

        cr_assert_null(actual->value);
        cr_assert_eq(actual->kind, keywords[i]);

        token_drop(actual);
    }

    lexer_drop(lexer);
}

Test(lexer, operators) {
    const int size = 31;

    lexer_t* lexer = new_lexer("test.cr", "( ) { } [ ] , ; :: -> "
                                          "+ - * / % | & ^ ! ~ ++ --"
                                          "= == != < > <= >= << >> || &&");

    token_kind_t keywords[size] = {
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
    };

    for (size_t i = 0; i < size; i++) {
        token_t* actual = lexer_next_token(lexer);

        cr_assert_null(actual->value);
        cr_assert_eq(actual->kind, keywords[i]);

        token_drop(actual);
    }

    lexer_drop(lexer);
}

Test(lexer, token_proximity) {
    const int size = 3;

    lexer_t* lexer = new_lexer("test.cr", "test()");

    token_kind_t keywords[size] = {
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
}
