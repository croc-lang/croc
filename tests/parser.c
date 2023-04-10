#include <criterion/criterion.h>
#include "../src/frontend/parser.h"

Test(parser, int) {
    lexer_t* lexer = new_lexer("test.cr", "1;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_INT_LITERAL);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, unary) {
    const int size = 7;

    expr_kind_t unaries[size] = {
        EX_UNA_INCR,
        EX_UNA_DECR,
        EX_UNA_NEG,
        EX_UNA_NOT,
        EX_UNA_BINNOT,
        EX_UNA_SUFFIX_INCR,
        EX_UNA_SUFFIX_DECR,
    };
    lexer_t* lexer = new_lexer("test.cr", "++1; --1; -1; !1; ~1; 1++; 1--;");
    parser_t* parser = new_parser(lexer);

    for (size_t i = 0; i < size; i++) {
        stmt_t* stmt = parser_next(parser);

        cr_assert_eq(stmt->kind, STMT_EXPR);
        cr_assert_eq(stmt->value.expr->kind, unaries[i]);
        cr_assert_eq(stmt->value.expr->value.unary->value->kind,
            EX_INT_LITERAL);
        cr_assert_str_eq(
            stmt->value.expr->value.unary->value->value.value->data,
            "1");

        stmt_drop(stmt);
    }

    parser_drop(parser);
}

Test(parser, binary_add) {
    lexer_t* lexer = new_lexer("test.cr", "1 + 2;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_BIN_ADD);

    cr_assert_eq(stmt->value.expr->value.binary->left->kind, EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.expr->value.binary->left->value.value->data,
        "1");

    cr_assert_eq(stmt->value.expr->value.binary->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.expr->value.binary->right->value.value->data,
        "2");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, orders_of_precedence) {
    lexer_t* lexer = new_lexer("test.cr", "3 * 1 + 2;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_BIN_ADD);

    cr_assert_eq(stmt->value.expr->value.binary->left->kind, EX_BIN_MUL);
    cr_assert_eq(
        stmt->value.expr->value.binary->left->value.binary->left->kind,
        EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.expr->value.binary->
            left->value.binary->left->value.value->data,
        "3");
    cr_assert_eq(
        stmt->value.expr->value.binary->left->value.binary->right->kind,
        EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.expr->value.binary->
            left->value.binary->right->value.value->data,
        "1");

    cr_assert_eq(stmt->value.expr->value.binary->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.expr->value.binary->right->value.value->data,
        "2");

    stmt_drop(stmt);
    parser_drop(parser);
}
