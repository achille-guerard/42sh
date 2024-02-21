#include <stdio.h>
#include <stdlib.h>

#include "../string_list/string_list.h"
#include "./includes/ast_create.h"

struct ast_case *ast_create_case(void)
{
    struct ast_case *res = calloc(1, sizeof(struct ast_case));
    if (!res)
        return NULL;

    ((struct ast_base *)(res))->type = AST_CASE;
    res->items = list_init(NULL);

    return res;
}

struct ast_case_item *ast_create_case_item(void)
{
    struct ast_case_item *res = calloc(1, sizeof(struct ast_case_item));
    if (!res)
        return NULL;

    ((struct ast_base *)(res))->type = AST_CASE_ITEM;
    res->patterns = string_list_init(NULL, NULL);

    return res;
}
