#include <stdio.h>
#include <frontend/parser.h>
#include <memory.h>
#include <string_utils.h>

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("USAGE: %s <file.cr>\n", argv[0]);
        return 1;
    }

    memory_container_t* container = new_memory_container();

    lexer_t* lexer = from_file_lexer(argv[1]);
    parser_t* parser = new_parser(lexer);

    parser_drop(parser);
    lexer_drop(lexer);

    memory_container_drop(container);
    return 0;
}
