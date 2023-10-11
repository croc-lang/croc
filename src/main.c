#include <stdio.h>
#include <frontend/parser.h>
#include <memory.h>
#include <string_utils.h>

int main(int argc, char** argv) {
    memory_container_t* container = new_memory_container();
    string_t* str = new_string("Hello");
    string_drop(str);
    memory_container_drop(container);
    // if (argc == 1) {
    //     printf("USAGE: %s <file.cr>\n", argv[0]);
    //     return 1;
    // }

    // lexer_t* lexer = from_file_lexer(argv[1]);
    // parser_t* parser = new_parser(lexer);
    // parser_drop(parser);

    return 0;
}
