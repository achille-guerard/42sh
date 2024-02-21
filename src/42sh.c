#include "unistd.h"
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

#include "assigner/assigner.h"
#include "ast/includes/ast.h"
#include "ast/includes/ast_evaluate.h"
#include "expander/expander.h"
#include "io/io.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

// read all the file and return a string containing the content
int main(int argc, char **argv)
{
    set_pwd();
    // token_pretty_print(IO_STDIN, NULL);
    //  struct ast_base *tree;
    FILE *fd;
    enum parser_status status = PARSER_OK;

    if (argc == 1)
    {
        // tree = parser_init(IO_STDIN, NULL, &status);
        init_shell_var(argc, argv, 0);
        return parser_loop(IO_STDIN, NULL, status);
    }
    else if ((fd = fopen(argv[1], "r")) != NULL) // argc == 2)
    {
        // tree = parser_init(IO_FILE, argv[1], &status);
        fclose(fd);
        init_shell_var(argc, argv, 1);
        return parser_loop(IO_FILE, argv[1], status);
    }
    else if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        // tree = parser_init(IO_STRING, argv[2], &status);
        init_shell_var(argc, argv, 0);
        return parser_loop(IO_STRING, argv[2], status);
    }
    else
    {
        fprintf(stderr, "Invalid input\n");
        fprintf(stderr, "Usage: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
        exit(2);
    }

    // int code = ast_eval(tree);
    // ast_free(tree);
    // return code;
}
