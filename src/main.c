#include <stdio.h>
#include <stdlib.h>
#include "frontend/lexer.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("USAGE: %s <file.cr>\n", argv[0]);
        return 1;
    }

    lexer_t* lexer = from_file_lexer(argv[1]);
    lexer_drop(lexer);

    return 0;
}
