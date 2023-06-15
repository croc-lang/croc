#include <criterion/criterion.h>
#include <frontend/parser.h>

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
    const int size = 7;

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

Test(parser, useless_semi_colon) {
    lexer_t* lexer = new_lexer("test.cr", ";;;1;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_EXPR);
    cr_assert_eq(stmt->value.expr->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.expr->value.value->data, "1");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, while_only) {
    lexer_t* lexer = new_lexer("test.cr", "while (a) b;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_WHILE);
    cr_assert_eq(stmt->value.if_stmt->condition->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.if_stmt->condition->value.value->data, "a");

    stmt_t* stmt2 = vector_get(stmt->value.if_stmt->body, 0);
    cr_assert_eq(stmt2->kind, STMT_EXPR);
    cr_assert_eq(stmt2->value.expr->kind, EX_IDENT);
    cr_assert_str_eq(stmt2->value.expr->value.value->data, "b");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, while_multi) {
    lexer_t* lexer = new_lexer("test.cr", "while (a) {\nb;b2;\n}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_WHILE);
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
    cr_assert_eq(stmt->value.var->public, 0);
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
    cr_assert_eq(stmt->value.var->public, 0);
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

Test(parser, var_with_type_declaration_without_initial_value) {
    lexer_t* lexer = new_lexer("test.cr", "int a;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->public, 0);
    cr_assert_eq(stmt->value.var->constant, false);

    cr_assert_eq(stmt->value.var->type->kind, TY_PATH);
    string_t* type_value = vector_get(
        stmt->value.var->type->value.path->segments,
        0);
    cr_assert_str_eq(type_value->data, "int");

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_null(stmt->value.var->right);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, var_with_tuple_type_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "(int, uint) a = (8, 4);");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->public, 0);
    cr_assert_eq(stmt->value.var->constant, false);

    type_t* first_type = vector_get(
        stmt->value.var->type->value.tuple,
        0);
    string_t* segment = vector_get(
        first_type->value.path->segments,
        0);
    cr_assert_str_eq(segment->data, "int");

    type_t* second_type = vector_get(
        stmt->value.var->type->value.tuple,
        1);
    string_t* segment2 = vector_get(
        second_type->value.path->segments,
        0);
    cr_assert_str_eq(segment2->data, "uint");

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_TUPLE);
    vector_t* list = stmt->value.var->right->value.list;
    expr_t* one = vector_get(list, 0);
    cr_assert_eq(one->kind, EX_INT_LITERAL);
    cr_assert_str_eq(one->value.value->data, "8");

    expr_t* two = vector_get(list, 1);
    cr_assert_eq(two->kind, EX_INT_LITERAL);
    cr_assert_str_eq(two->value.value->data, "4");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, const_with_let_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "const let a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->public, 0);
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
    cr_assert_eq(stmt->value.var->public, 0);
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

Test(parser, public_var_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "pub let a = 8;");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_VAR_DECLARATION);
    cr_assert_eq(stmt->value.var->public, 1);
    cr_assert_eq(stmt->value.var->constant, false);
    cr_assert_null(stmt->value.var->type);

    cr_assert_eq(stmt->value.var->left->kind, EX_IDENT);
    cr_assert_str_eq(stmt->value.var->left->value.value->data, "a");

    cr_assert_eq(stmt->value.var->right->kind, EX_INT_LITERAL);
    cr_assert_str_eq(stmt->value.var->right->value.value->data, "8");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, public_func_definition) {
    lexer_t* lexer = new_lexer("test.cr", "pub func test() {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 1);
    cr_assert_null(stmt->value.func->return_type);
    cr_assert_eq(stmt->value.func->args->len, 0);
    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_definition) {
    lexer_t* lexer = new_lexer("test.cr", "func test() {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 0);
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
    cr_assert_eq(stmt->value.func->public, 0);
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

Test(parser, func_with_return_pointer_type) {
    lexer_t* lexer = new_lexer("test.cr", "func test() int* {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 0);
    cr_assert_eq(stmt->value.func->return_type->kind, TY_POINTER);
    cr_assert_eq(stmt->value.func->return_type->value.type->kind, TY_PATH);
    string_t* segment = vector_get(
        stmt->value.func->return_type->value.type->value.path->segments,
        0);
    cr_assert_str_eq(segment->data, "int");

    cr_assert_eq(stmt->value.func->args->len, 0);
    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_with_return_type_with_parent) {
    lexer_t* lexer = new_lexer("test.cr", "func test() (int) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 0);
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

Test(parser, func_with_return_1_tuple_type) {
    lexer_t* lexer = new_lexer("test.cr", "func test() (int,) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 0);
    cr_assert_eq(stmt->value.func->return_type->kind, TY_TUPLE);
    type_t* first_type = vector_get(
        stmt->value.func->return_type->value.tuple,
        0);
    string_t* segment = vector_get(
        first_type->value.path->segments,
        0);
    cr_assert_str_eq(segment->data, "int");

    cr_assert_eq(stmt->value.func->args->len, 0);
    cr_assert_eq(stmt->value.func->body->len, 0);
    cr_assert_str_eq(stmt->value.func->name->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, func_with_return_more_tuple_type) {
    lexer_t* lexer = new_lexer("test.cr", "func test() (int, uint) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FUNC_DEFINITION);
    cr_assert_eq(stmt->value.func->public, 0);
    cr_assert_eq(stmt->value.func->return_type->kind, TY_TUPLE);
    type_t* first_type = vector_get(
        stmt->value.func->return_type->value.tuple,
        0);
    string_t* segment = vector_get(
        first_type->value.path->segments,
        0);
    cr_assert_str_eq(segment->data, "int");

    type_t* second_type = vector_get(
        stmt->value.func->return_type->value.tuple,
        1);
    string_t* segment2 = vector_get(
        second_type->value.path->segments,
        0);
    cr_assert_str_eq(segment2->data, "uint");

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
    cr_assert_eq(stmt->value.func->public, 0);
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
    cr_assert_eq(stmt->value.func->public, 0);
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

Test(parser, module) {
    lexer_t* lexer = new_lexer("test.cr", "module test");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_MODULE);
    cr_assert_eq(stmt->value.module->path->segments->len, 1);
    string_t* segment = vector_get(stmt->value.module->path->segments, 0);
    cr_assert_str_eq(segment->data, "test");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, long_module) {
    lexer_t* lexer = new_lexer("test.cr", "module test::test2");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_MODULE);
    string_t* segment = vector_get(stmt->value.module->path->segments, 0);
    cr_assert_str_eq(segment->data, "test");
    string_t* segment2 = vector_get(stmt->value.module->path->segments, 1);
    cr_assert_str_eq(segment2->data, "test2");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, import_a_file) {
    lexer_t* lexer = new_lexer("test.cr", "import \"./test2.cr\"");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    cr_assert_null(import->move_to);
    cr_assert_null(import->imports);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, import_multiple_files) {
    lexer_t* lexer = new_lexer(
        "test.cr",
        "import \"./test2.cr\", \"./test3.cr\"");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    cr_assert_null(import->move_to);
    cr_assert_null(import->imports);

    import_stmt_t* import2 = vector_get(stmt->value.imports, 1);
    cr_assert_str_eq(import2->file_paths->data, "./test3.cr");
    cr_assert_null(import2->move_to);
    cr_assert_null(import2->imports);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, import_with_rename) {
    lexer_t* lexer = new_lexer("test.cr", "import \"./test2.cr\" as test");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    cr_assert_str_eq(import->move_to->data, "test");
    cr_assert_null(import->imports);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, import_specific_object) {
    lexer_t* lexer = new_lexer("test.cr", "import \"./test2.cr\" { test }");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    string_t* object = vector_get(import->imports, 0);
    cr_assert_str_eq(object->data, "test");
    cr_assert_null(import->move_to);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, import_specific_many_objects) {
    lexer_t* lexer = new_lexer(
        "test.cr",
        "import \"./test2.cr\" { test, test2 }");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    string_t* object = vector_get(import->imports, 0);
    cr_assert_str_eq(object->data, "test");
    string_t* object2 = vector_get(import->imports, 1);
    cr_assert_str_eq(object2->data, "test2");
    cr_assert_null(import->move_to);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, imports_with_rename_and_publish_get) {
    lexer_t* lexer = new_lexer(
        "test.cr",
        "import \"./test2.cr\" as test, \"./test3.cr\" { test4 }");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_IMPORTS);
    import_stmt_t* import = vector_get(stmt->value.imports, 0);
    cr_assert_str_eq(import->file_paths->data, "./test2.cr");
    cr_assert_str_eq(import->move_to->data, "test");
    cr_assert_null(import->imports);

    import_stmt_t* import2 = vector_get(stmt->value.imports, 1);
    cr_assert_str_eq(import2->file_paths->data, "./test3.cr");
    string_t* object = vector_get(import2->imports, 0);
    cr_assert_str_eq(object->data, "test4");
    cr_assert_null(import2->move_to);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, for_each) {
    lexer_t* lexer = new_lexer("test.cr", "for (let a in []) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FOR);
    cr_assert_eq(stmt->value.for_stmt->body->len, 0);
    cr_assert_eq(stmt->value.for_stmt->kind, FK_EACH);

    cr_assert_eq(stmt->value.for_stmt->value.each->constant, false);
    cr_assert_null(stmt->value.for_stmt->value.each->type);

    cr_assert_eq(stmt->value.for_stmt->value.each->left->kind, EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.each->left->value.value->data,
        "a");

    cr_assert_eq(stmt->value.for_stmt->value.each->right->kind, EX_ARRAY);
    cr_assert_eq(stmt->value.for_stmt->value.each->right->value.list->len, 0);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, for_empty_primary) {
    lexer_t* lexer = new_lexer("test.cr", "for (;;) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FOR);
    cr_assert_eq(stmt->value.for_stmt->body->len, 0);
    cr_assert_eq(stmt->value.for_stmt->kind, FK_PRIMARY);

    cr_assert_eq(stmt->value.for_stmt->value.primary->init_kind, FOR_INIT_NONE);
    cr_assert_null(stmt->value.for_stmt->value.primary->condition);
    cr_assert_null(stmt->value.for_stmt->value.primary->updater);

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, for_primary) {
    lexer_t* lexer = new_lexer("test.cr", "for (a; a; a) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FOR);
    cr_assert_eq(stmt->value.for_stmt->body->len, 0);
    cr_assert_eq(stmt->value.for_stmt->kind, FK_PRIMARY);

    cr_assert_eq(stmt->value.for_stmt->value.primary->init_kind, FOR_INIT_EXPR);
    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init.expr->kind,
        EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->init.expr->value.value->data,
        "a");

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->condition->kind,
        EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->condition->value.value->data,
        "a");

    cr_assert_eq(stmt->value.for_stmt->value.primary->updater->kind, EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->updater->value.value->data,
        "a");

    stmt_drop(stmt);
    parser_drop(parser);
}

Test(parser, for_primary_with_declaration) {
    lexer_t* lexer = new_lexer("test.cr", "for (int a = 0; a; a) {}");
    parser_t* parser = new_parser(lexer);

    stmt_t* stmt = parser_next(parser);

    cr_assert_eq(stmt->kind, STMT_FOR);
    cr_assert_eq(stmt->value.for_stmt->body->len, 0);
    cr_assert_eq(stmt->value.for_stmt->kind, FK_PRIMARY);

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init_kind,
        FOR_INIT_DECLA);

    cr_assert_eq(stmt->value.for_stmt->value.primary->init.decla->public, 0);
    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init.decla->constant,
        false);

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init.decla->type->kind,
        TY_PATH);
    string_t* type_value = vector_get(
        stmt->value.for_stmt->value.primary->
            init.decla->type->value.path->segments,
        0);
    cr_assert_str_eq(type_value->data, "int");

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init.decla->left->kind,
        EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->
            init.decla->left->value.value->data,
        "a");

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->init.decla->right->kind,
        EX_INT_LITERAL);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->
            init.decla->right->value.value->data,
        "0");

    cr_assert_eq(
        stmt->value.for_stmt->value.primary->condition->kind,
        EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->condition->value.value->data,
        "a");

    cr_assert_eq(stmt->value.for_stmt->value.primary->updater->kind, EX_IDENT);
    cr_assert_str_eq(
        stmt->value.for_stmt->value.primary->updater->value.value->data,
        "a");

    stmt_drop(stmt);
    parser_drop(parser);
}
