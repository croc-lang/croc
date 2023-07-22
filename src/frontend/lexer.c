#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <frontend/lexer.h>
#include <string_utils.h>

typedef struct {
    token_kind_t kind;
    char* text;
} hardcoded_token;

static const hardcoded_token tokens[] = {
        {TK_ASG_BIT_SL, "<<="},
        {TK_ASG_BIT_SR, ">>="},
        {TK_ASG_BOOL_AND, "&&="},
        {TK_ASG_BOOL_OR, "||="},
        {TK_CMP_EQ, "=="},
        {TK_CMP_NE, "!="},
        {TK_CMP_GE, ">="},
        {TK_CMP_LE, "<="},
        {TK_ASG_PLUS, "+="},
        {TK_ASG_MINUS, "-="},
        {TK_ASG_STAR, "*="},
        {TK_ASG_SLASH, "/="},
        {TK_ASG_PERCENT, "%="},
        {TK_ASG_BIT_AND, "&="},
        {TK_ASG_BIT_OR, "|="},
        {TK_ASG_BIT_XOR, "^="},
        {TK_BIT_SL, "<<"},
        {TK_BIT_SR, ">>"},
        {TK_BOOL_AND, "&&"},
        {TK_BOOL_OR, "||"},
        {TK_DB_COLON, "::"},
        {TK_DECREMENT, "--"},
        {TK_INCREMENT, "++"},
        {TK_ARROW, "->"},
        {TK_PLUS, "+"},
        {TK_MINUS, "-"},
        {TK_STAR, "*"},
        {TK_SLASH, "/"},
        {TK_PERCENT, "%"},
        {TK_BIN_AND, "&"},
        {TK_BIN_OR, "|"},
        {TK_BIN_XOR, "^"},
        {TK_EQ, "="},
        {TK_BANG, "!"},
        {TK_TILDE, "~"},
        {TK_CMP_LT, "<"},
        {TK_CMP_GT, ">"},
        {TK_SEMICOLON, ";"},
        {TK_COMMA, ","},
        {TK_LPAREN, "("},
        {TK_RPAREN, ")"},
        {TK_LBRACE, "{"},
        {TK_RBRACE, "}"},
        {TK_LBRACKET, "["},
        {TK_RBRACKET, "]"},
};
static const size_t tokens_count = sizeof(tokens) / sizeof(hardcoded_token);

static const hardcoded_token keywords_tokens[] = {
    {TK_KW_LET, "let"},
    {TK_KW_IF, "if"},
    {TK_KW_ELSE, "else"},
    {TK_KW_FUNC, "func"},
    {TK_KW_CONST, "const"},
    {TK_KW_FOR, "for"},
    {TK_KW_WHILE, "while"},
    {TK_KW_PUBLIC, "pub"},
    {TK_KW_IN, "in"},
    {TK_KW_IMPORT, "import"},
    {TK_KW_MODULE, "module"},
    {TK_KW_RETURN, "return"},
    {TK_KW_AS, "as"},
    {TK_KW_STRUCT, "struct"},
};
static const size_t keywords_tokens_count =
    sizeof(keywords_tokens) / sizeof(hardcoded_token);

lexer_t* from_file_lexer(char* path) {
    lexer_t* lexer = calloc(sizeof(lexer_t), 1);
    string_t* src = NULL;
    size_t len = 0;

    FILE* file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);
    src = sized_string(len);

    if (src == NULL)
        return NULL;

    fread(src->data, len, 1, file);
    src->len = len;

    lexer->context = new_context();
    lexer->filename = path;
    lexer->src = src;
    lexer->i = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->prev_col = 0;

    fclose(file);

    return lexer;
}

lexer_t* new_lexer(char* path, char* src) {
    lexer_t* lexer = calloc(sizeof(lexer_t), 1);
    string_t* string_src = new_string(src);

    if (string_src == NULL)
        return NULL;

    lexer->context = new_context();
    lexer->filename = path;
    lexer->src = string_src;
    lexer->i = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->prev_col = 0;

    return lexer;
}

static size_t lexer_forward(lexer_t* self) {
    size_t index = self->i++;
    self->prev_col = self->col;
    if (string_get(self->src, self->i) == '\n') {
        self->col = 1;
        self->line += 1;
    } else self->col += 1;
    return index;
}

static size_t lexer_backward(lexer_t* self) {
    if (self->col == 1) self->line -= 1;
    self->col = self->prev_col;
    return --self->i;
}

static token_t* lexing_ident(lexer_t* lexer, size_t start) {
    size_t line = lexer->line, col = lexer->col;
    while (isalnum(string_get(lexer->src, lexer_forward(lexer))));

    string_t* ident = string_slice(lexer->src, start, lexer_backward(lexer));

    for (size_t i = 0; i < keywords_tokens_count; i++) {
        if (string_eq_str(ident, keywords_tokens[i].text)) {
            string_drop(ident);
            return new_token(keywords_tokens[i].kind,
                NULL,
                new_location(line, col, start, lexer->i));
        }
    }

    return new_token(TK_IDENT,
        ident,
        new_location(line, col, start, lexer->i));
}

static token_t* lexing_number(lexer_t* lexer, size_t start) {
    size_t line = lexer->line, col = lexer->col, first_occ = 0;
    token_kind_t kind = TK_INT;
    bool error = false;
    string_t* slice;
    char c;
    while ((c = string_get(lexer->src, lexer_forward(lexer)), isalnum(c))) {
        if (isalpha(c) && !error) {
            error = true;
            first_occ = lexer->i - 1;
        }
    }

    c = string_get(lexer->src, lexer->i - 1);
    if (c > 0 && c == '.' && isdigit(string_get(lexer->src, lexer->i))) {
        kind = TK_FLOAT;
        while ((c = string_get(lexer->src, lexer_forward(lexer)), isalnum(c))) {
            if (isalpha(c) && !error) {
                error = true;
                first_occ = lexer->i - 1;
            }
        }
    }

    if (error) {
        slice = string_slice(lexer->src,
            start,
            lexer_backward(lexer));
        context_add_error(lexer->context,
            new_error(CTX_ERR_INVALID_SYNTAX,
                format_string("`%s` is invalid"
                    ", we didn't expect to have '%c' in the number"
                    ", try rewriting it.",
                    slice->data,
                    string_get(lexer->src, first_occ)),
                new_location(line, col, start, lexer->i)));
        string_drop(slice);
    }

    return new_token(kind,
        string_slice(lexer->src, start, lexer_backward(lexer)),
        new_location(line, col, start, lexer->i));
}

static token_t* lexing_string(lexer_t* lexer, size_t start) {
    size_t line = lexer->line, col = lexer->col;
    string_t* str = new_string("");
    char c;

    while ((c = string_get(lexer->src, lexer_forward(lexer))) > 0) {
        if (c == '\\') {
            switch ((c = string_get(lexer->src, lexer_forward(lexer)))) {
            case '0':
                string_push_char(str, '\0');
                break;
            case 'n':
                string_push_char(str, '\n');
                break;
            case 'r':
                string_push_char(str, '\r');
                break;
            case 't':
                string_push_char(str, '\t');
                break;
            case 'b':
                string_push_char(str, '\b');
                break;
            case 'f':
                string_push_char(str, '\f');
                break;
            case 'v':
                string_push_char(str, '\v');
                break;
            case '\n':
                break;
            default:
                string_push_char(str, c);
                break;
            }
        } else if (c == '"') break;
        else string_push_char(str, c);
    }

    if (c != '"') context_add_error(lexer->context,
        new_error(CTX_ERR_UNTERMINATED_STR,
            new_string("not ended string"),
            new_location(line, col, start, lexer->i)));

    return new_token(TK_STRING, str, new_location(line, col, start, lexer->i));
}

token_t* lexer_next_token(lexer_t* self) {
    size_t line, col, start;
    size_t incrementor;
    char c;
    while ((c = string_get(self->src, lexer_forward(self))) > 0) {
        line = self->line;
        col = self->col;
        start = self->i - 1;

        if (!isascii(c)) {
            while (!isascii(string_get(self->src, lexer_forward(self))));
            string_t* slice = string_slice(self->src, start,
                lexer_backward(self));

            context_add_error(self->context,
                new_error(CTX_ERR_INVALID_SLICE,
                    format_string("`%s` was invalid", slice->data),
                    new_location(line, col, start, self->i)));
            string_drop(slice);
            continue;
        } else if (isblank(c) || isspace(c)) continue;
        else if (isdigit(c)) return lexing_number(self, self->i - 1);
        else if (isalpha(c)) return lexing_ident(self, self->i - 1);
        else if (c == '"') return lexing_string(self, self->i - 1);
        else if (string_offset_start_with(self->src, "//", start)) {
            do { c = string_get(self->src, lexer_forward(self)); }
            while (c != -1 && c != '\n');
            continue;
        }

        for (size_t i = 0; i < tokens_count; i++) {
            if (string_offset_start_with(self->src, tokens[i].text, start)) {
                incrementor = strlen(tokens[i].text) - 1;
                self->i += incrementor;
                self->prev_col = self->col;
                self->col += incrementor;
                return new_token(
                    tokens[i].kind,
                    NULL,
                    new_location(line, col, start, self->i));
            }
        }
    }

    return new_token(TK_EOF,
        NULL,
        new_location(line, col, self->src->len, self->src->len));
}

void lexer_goto_location(lexer_t* self, location_t* location) {
    self->col = location->col;
    self->line = location->line;
    self->i = location->start;
}

void lexer_drop(lexer_t* self) {
    string_drop(self->src);
    free(self);
}
