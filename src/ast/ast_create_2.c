#include <stdio.h>
#include <stdlib.h>

#include "../string_list/string_list.h"
#include "./includes/ast_create.h"

struct ast_negation *ast_creation_negation(void)
{
    struct ast_negation *res = calloc(1, sizeof(struct ast_negation));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_NEGATION;
    return res;
}

struct ast_and_or *ast_create_and_or(void)
{
    struct ast_and_or *res = calloc(1, sizeof(struct ast_and_or));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_AND_OR;

    return res;
}

struct ast_loop *ast_create_loop(void)
{
    struct ast_loop *res = calloc(1, sizeof(struct ast_loop));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_LOOP;

    return res;
}

struct ast_for *ast_create_for(void)
{
    struct ast_for *res = calloc(1, sizeof(struct ast_for));
    if (!res)
        return NULL;

    res->words = calloc(8, sizeof(char *));
    res->capacity = 8;
    res->size = 0;

    ((struct ast_base *)(res))->type = AST_FOR;

    return res;
}

struct ast_funcdec *ast_create_funcdec(void)
{
    struct ast_funcdec *res = calloc(1, sizeof(struct ast_funcdec));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_FUNCDEC;
    return res;
}
