#include <unistd.h>

#include "../string_list/string_list.h"
#include "./includes/ast_free.h"
#include "includes/ast.h"

const ast_free_func op_free[] = { [AST_COMMAND_LIST] = ast_free_command_list,
                                  [AST_RULE_IF] = ast_free_rule_if,
                                  [AST_COMPOUND_LIST] = ast_free_compound_list,
                                  [AST_SIMPLE_COMMAND] =
                                      ast_free_simple_command,
                                  [AST_REDIRECTION] = ast_free_redirection,
                                  [AST_PIPELINE] = ast_free_pipeline,
                                  [AST_NEGATION] = ast_free_negation,
                                  [AST_AND_OR] = ast_free_and_or,
                                  [AST_LOOP] = ast_free_loop,
                                  [AST_COMMAND] = ast_free_command,
                                  [AST_FOR] = ast_free_for,
                                  [AST_FUNCDEC] = ast_free_funcdec,
                                  [AST_CASE] = ast_free_case,
                                  [AST_CASE_ITEM] = ast_free_case_item };

static void manage_redirs_free(struct list *redirections)
{
    if (redirections == NULL || redirections->child == NULL)
        return;
    manage_redirs_free(redirections->next);
    struct ast_redirection *ast = (struct ast_redirection *)redirections->child;

    // dup2 closes right arg if
    // already used somewhere
    dup2(ast->fd_backup, ast->fd_to_restore);
    close(ast->fd_backup);
    if (STDIN_FILENO == ast->fd_open || STDOUT_FILENO == ast->fd_open
        || STDERR_FILENO == ast->fd_open)
        return;
    close(ast->fd_open);
}

void ast_free_simple_command(struct ast_base *ast)
{
    struct ast_simple_command *as = (struct ast_simple_command *)ast;
    if (!as)
        return;

    string_list_free(as->commands);
    string_list_free(as->assignation);
    string_list_free(as->expanded);

    if (as->redirections)
    {
        manage_redirs_free(as->redirections);
        list_free(as->redirections);
    }
    free(as);
}

void ast_free_rule_if(struct ast_base *ast)
{
    struct ast_rule_if *as = (struct ast_rule_if *)ast;
    if (!as)
        return;
    if (as->left)
        op_free[as->left->type](as->left);
    if (as->right)
        op_free[as->right->type](as->right);
    if (as->middle)
        op_free[as->middle->type](as->middle);
    as->right = NULL;
    as->left = NULL;
    as->middle = NULL;

    free(as);
}

void ast_free_command_list(struct ast_base *ast)
{
    struct ast_command_list *as = (struct ast_command_list *)ast;
    if (!as)
        return;
    list_free(as->commands);
    as->commands = NULL;
    free(as);
}

void ast_free_compound_list(struct ast_base *ast)
{
    struct ast_compound_list *as = (struct ast_compound_list *)ast;
    if (!as)
        return;
    list_free(as->commands);
    as->commands = NULL;
    as->commands = NULL;
    free(as);
}

void ast_free_redirection(struct ast_base *ast)
{
    struct ast_redirection *as = (struct ast_redirection *)ast;
    free(as->destination);
    free(as->redir_type);
    // free(as->fd);
    free(as);
}

void ast_free_pipeline(struct ast_base *ast)
{
    // printf("free pipeline %p\n",(void *)ast);
    struct ast_pipeline *as = (struct ast_pipeline *)ast;
    if (as->left)
        op_free[as->left->type](as->left);
    if (as->right)
        op_free[as->right->type](as->right);
    free(as);
}
