#include <unistd.h>

#include "../string_list/string_list.h"
#include "./includes/ast_free.h"
#include "includes/ast.h"

void ast_free_case(struct ast_base *ast)
{
    struct ast_case *as = (struct ast_case *)ast;
    list_free(as->items);
    free(as->pattern);
    free(as);
}

void ast_free_case_item(struct ast_base *ast)
{
    struct ast_case_item *as = (struct ast_case_item *)ast;
    string_list_free(as->patterns);
    ast_free(as->instruction);
    free(as);
}
