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
    const int const size = 7;

    expr_kind_t unaries[] = {
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

Test(parser, var_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "let a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->constant, false);
    cr_assert_null(stmt->value.var->type);

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.var->right->value.value->data, "8");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, var_with_type_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "int a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->constant, false);

    cr_assert_eq(stmt->value.var->type->kind, TY_PATH);
    string_t* type_value = vector_get(
        stmt->value.var->type->value.path->segments,
        0);
    cr_assert_str_eq(type_value->data, "int");

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.var->right->value.value->data, "8");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, const_with_let_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "const let a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->constant, true);
    cr_assert_null(stmt->value.var->type);

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.var->right->value.value->data, "8");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, const_with_type_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "const int a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->constant, true);

    cr_assert_eq(stmt->value.var->type->kind, TY_PATH);
    string_t* type_value = vector_get(
        stmt->value.var->type->value.path->segments,
        0);
    cr_assert_str_eq(type_value->data, "int");

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.var->right->value.value->data, "8");

    stmt_drop(stmt);
    parser_drop(parser);
}
