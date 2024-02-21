#ifndef PARSER_H
#define PARSER_H

#include "../ast/includes/ast.h"
#include "../ast/includes/ast_create.h"
#include "../ast/includes/ast_evaluate.h"
#include "../ast/includes/ast_free.h"
#include "../io/io.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_EOF,
    PARSER_UNEXPECTED_TOKEN,
    PARSER_NEWLINE,
};

struct ast_base *parser_init(enum parser_status *stat, struct lexer *lexer);
int parser_loop(enum mode mode, char *file, enum parser_status status);

#endif /* !PARSER_H */
