#include <criterion/criterion.h>
#include "../src/frontend/parser.h"

Test(parser, int) {
    lexer_t* lexer = new_lexer("test.cr", "1;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.expr->value.value->data, "1");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, int_in_parent) {
    lexer_t* lexer = new_lexer("test.cr", "(1);");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.expr->value.value->data, "1");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, tuple_with_1_element) {
    lexer_t* lexer = new_lexer("test.cr", "(1,);");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_TUPLE);

    vector_t* list = stmt->value.expr->value.list;
    expr_t* one = vector_get(list, 0);
    cr_assert_eq(one->kind, EX_INT_LITERAL);
    cr_assert_str_eq(one->value.value->data, "1");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, tuple_with_more_elements) {
    lexer_t* lexer = new_lexer("test.cr", "(1, 2, 3);");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_TUPLE);

    vector_t* list = stmt->value.expr->value.list;
    expr_t* one = vector_get(list, 0);
    cr_assert_eq(one->kind, EX_INT_LITERAL);
    cr_assert_str_eq(one->value.value->data, "1");

    expr_t* two = vector_get(list, 1);
    cr_assert_eq(two->kind, EX_INT_LITERAL);
    cr_assert_str_eq(two->value.value->data, "2");

    expr_t* three = vector_get(list, 2);
    cr_assert_eq(three->kind, EX_INT_LITERAL);
    cr_assert_str_eq(three->value.value->data, "3");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, array_with_no_element) {
    lexer_t* lexer = new_lexer("test.cr", "[];");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_ARRAY);
    cr_assert_eq(stmt->value.expr->value.list->len, 0);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, array_with_1_element) {
    lexer_t* lexer = new_lexer("test.cr", "[1];");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_ARRAY);

    vector_t* list = stmt->value.expr->value.list;
    expr_t* one = vector_get(list, 0);
    cr_assert_eq(one->kind, EX_INT_LITERAL);
    cr_assert_str_eq(one->value.value->data, "1");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, array_with_more_elements) {
    lexer_t* lexer = new_lexer("test.cr", "[1, 2, 3];");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_ARRAY);

    vector_t* list = stmt->value.expr->value.list;
    expr_t* one = vector_get(list, 0);
    cr_assert_eq(one->kind, EX_INT_LITERAL);
    cr_assert_str_eq(one->value.value->data, "1");

    expr_t* two = vector_get(list, 1);
    cr_assert_eq(two->kind, EX_INT_LITERAL);
    cr_assert_str_eq(two->value.value->data, "2");

    expr_t* three = vector_get(list, 2);
    cr_assert_eq(three->kind, EX_INT_LITERAL);
    cr_assert_str_eq(three->value.value->data, "3");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, string_auto_concat) {
    lexer_t* lexer = new_lexer("test.cr", "\"hello\" \" world\";");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_STRING_LITERAL);
    cr_assert_str_eq(stmt->value.expr->value.value->data, "hello world");

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

Test(parser, if_only) {
    lexer_t* lexer = new_lexer("test.cr", "if (a) b;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IF);
    cr_assert_eq(stmt->value.if_stmt->condition->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.if_stmt->condition->value.value->data, "a");

    stmt_t* stmt2 = vector_get(stmt->value.if_stmt->body, 0);
    cr_assert_eq(stmt2->kind, STMT_EXPR);
    cr_assert_eq(stmt2->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt2->value.expr->value.value->data, "b");

    cr_assert_null(stmt->value.if_stmt->else_branch);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, if_with_else) {
    lexer_t* lexer = new_lexer("test.cr", "if (a) b; else c;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IF);
    cr_assert_eq(stmt->value.if_stmt->condition->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.if_stmt->condition->value.value->data, "a");

    stmt_t* stmt2 = vector_get(stmt->value.if_stmt->body, 0);
    cr_assert_eq(stmt2->kind, STMT_EXPR);
    cr_assert_eq(stmt2->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt2->value.expr->value.value->data, "b");

    cr_assert_null(stmt->value.if_stmt->else_branch->if_branch);
    stmt_t* stmt3 = vector_get(stmt->value.if_stmt->else_branch->body, 0);
    cr_assert_eq(stmt3->kind, STMT_EXPR);
    cr_assert_eq(stmt3->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt3->value.expr->value.value->data, "c");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, if_with_else_if) {
    lexer_t* lexer = new_lexer(
        "test.cr",
        "if (a) {\nb;b2;\n} else if (c) {\nd;\n}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IF);
    cr_assert_eq(stmt->value.if_stmt->condition->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.if_stmt->condition->value.value->data, "a");

    stmt_t* stmt2 = vector_get(stmt->value.if_stmt->body, 0);
    cr_assert_eq(stmt2->kind, STMT_EXPR);
    cr_assert_eq(stmt2->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt2->value.expr->value.value->data, "b");

    stmt_t* stmt3 = vector_get(stmt->value.if_stmt->body, 1);
    cr_assert_eq(stmt3->kind, STMT_EXPR);
    cr_assert_eq(stmt3->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt3->value.expr->value.value->data, "b2");

    if_stmt_t* if_branch = stmt->value.if_stmt->else_branch->if_branch;
    cr_assert_null(stmt->value.if_stmt->else_branch->body);
    cr_assert_null(if_branch->else_branch);
    cr_assert_eq(if_branch->condition->kind, EX_IDENT);
    cr_assert_str_eq(if_branch->condition->value.value->data, "c");
    stmt_t* stmt4 = vector_get(if_branch->body, 0);
    cr_assert_eq(stmt4->kind, STMT_EXPR);
    cr_assert_eq(stmt4->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt4->value.expr->value.value->data, "d");

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

Test(parser, func_definition) {
    lexer_t* lexer = new_lexer("test.cr", "func test() {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_null(stmt->value.func->return_type);
    cr_assert_eq(stmt->value.func->args->len, 0);
    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_with_return_type) {
    lexer_t* lexer = new_lexer("test.cr", "func test() int {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->return_type->kind, TY_PATH);
    string_t* segment = vector_get(
        stmt->value.func->return_type->value.path->segments,
        0);
    cr_assert_str_eq(segment->data, "int");

    cr_assert_eq(stmt->value.func->args->len, 0);
    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_with_1_argument) {
    lexer_t* lexer = new_lexer("test.cr", "func test(int a) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_null(stmt->value.func->return_type);

    arg_expr_t* arg = vector_get(
        stmt->value.func->args,
        0);
    cr_assert_str_eq(arg->name->data, "a");
    cr_assert_eq(arg->type->kind, TY_PATH);
    string_t* segment = vector_get(arg->type->value.path->segments, 0);
    cr_assert_str_eq(segment->data, "int");

    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_with_more_arguments) {
    lexer_t* lexer = new_lexer("test.cr", "func test(int a, int b) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_null(stmt->value.func->return_type);

    arg_expr_t* arg = vector_get(
        stmt->value.func->args,
        0);
    cr_assert_str_eq(arg->name->data, "a");
    cr_assert_eq(arg->type->kind, TY_PATH);
    string_t* segment = vector_get(arg->type->value.path->segments, 0);
    cr_assert_str_eq(segment->data, "int");

    arg_expr_t* arg2 = vector_get(
        stmt->value.func->args,
        1);
    cr_assert_str_eq(arg2->name->data, "b");
    cr_assert_eq(arg2->type->kind, TY_PATH);
    string_t* segment2 = vector_get(arg2->type->value.path->segments, 0);
    cr_assert_str_eq(segment2->data, "int");

    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}
