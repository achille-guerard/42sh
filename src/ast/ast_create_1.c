#include <stdio.h>
#include <stdlib.h>

#include "../string_list/string_list.h"
#include "./includes/ast_create.h"

struct ast_command *ast_create_command(void)
{
    struct ast_command *res = calloc(1, sizeof(struct ast_command));
    if (!res)
        return NULL;

    res->redirections = list_init(NULL);
    ((struct ast_base *)(res))->type = AST_COMMAND;

    return res;
}

struct ast_simple_command *ast_create_simple_command(void)
{
    struct ast_simple_command *res =
        calloc(1, sizeof(struct ast_simple_command));
    if (!res)
        return NULL;
    res->redirections = list_init(NULL);
    res->commands = string_list_init(NULL, NULL);
    res->assignation = string_list_init(NULL, NULL);
    ((struct ast_base *)(res))->type = AST_SIMPLE_COMMAND;
    return res;
}

struct ast_command_list *ast_create_command_list(void)
{
    struct ast_command_list *res = calloc(1, sizeof(struct ast_command_list));
    if (!res)
        return NULL;
    res->commands = list_init(NULL);
    ((struct ast_base *)(res))->type = AST_COMMAND_LIST;

    return res;
}

struct ast_rule_if *ast_create_rule_if(void)
{
    struct ast_rule_if *res = calloc(1, sizeof(struct ast_rule_if));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_RULE_IF;

    return res;
}

struct ast_compound_list *ast_create_compound_list(void)
{
    struct ast_compound_list *res = calloc(1, sizeof(struct ast_compound_list));
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_COMPOUND_LIST;

    res->commands = list_init(NULL);
    return res;
}

struct ast_redirection *ast_create_redirection(void)
{
    struct ast_redirection *res = calloc(1, sizeof(struct ast_redirection));
    if (!res)
        return NULL;

    res->fd_backup = -1;
    res->fd_to_restore = -1;
    res->io_number = -1;
    res->fd_open = -1;
    ((struct ast_base *)(res))->type = AST_REDIRECTION;
    return res;
}

struct ast_pipeline *ast_create_pipeline(void)
{
    struct ast_pipeline *res = calloc(1, sizeof(struct ast_pipeline));
    // printf("pipeline %p\n",(void *)res);
    if (!res)
        return NULL;
    ((struct ast_base *)(res))->type = AST_PIPELINE;
    return res;
}
