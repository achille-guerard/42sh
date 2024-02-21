#include <unistd.h>

#include "../string_list/string_list.h"
#include "./includes/ast_free.h"
#include "includes/ast.h"

void ast_free_negation(struct ast_base *ast)
{
    struct ast_negation *as = (struct ast_negation *)ast;
    ast_free(as->to_negate);
    free(as);
}

void ast_free_and_or(struct ast_base *ast)
{
    struct ast_and_or *as = (struct ast_and_or *)ast;
    // free the vector list

    free(as->op_type);
    ast_free(as->left);
    ast_free(as->right);

    free(as);
}

void ast_free_loop(struct ast_base *ast)
{
    struct ast_loop *tree = (struct ast_loop *)ast;
    ast_free(tree->condition);
    ast_free(tree->instruction);
    free(tree);
}
/*
void ast_free_for(struct ast_base *ast)
{
    struct ast_for *as = (struct ast_for *)ast;
    ast_free(as->instruction);
    free(as->name);
    if (as->words)
    {
        for (size_t i = 0; as->words[i]; i++)
        {
            free(as->words[i]);
        }
        free(as->words);
    }
}*/

void ast_free_command(struct ast_base *ast)
{
    struct ast_command *as = (struct ast_command *)ast;
    ast_free(as->child);
    list_free(as->redirections);
    free(as);
}

void ast_free_for(struct ast_base *ast)
{
    struct ast_for *tree = (struct ast_for *)ast;
    free(tree->name);
    size_t i = 0;
    while (tree->words[i] != NULL)
    {
        free(tree->words[i]);
        i++;
    }
    free(tree->words);
    ast_free(tree->instruction);
    free(tree);
}

void ast_free_funcdec(struct ast_base *ast)
{
    struct ast_funcdec *tree = (struct ast_funcdec *)ast;
    free(tree->fname);
    // ast_free(tree->shell_command);
    free(tree);
}

void ast_free(struct ast_base *tree)
{
    if (!tree)
        return;
    op_free[tree->type](tree);
}
