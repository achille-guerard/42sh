#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "../assigner/assigner.h"
#include "../builtin/builtin.h"
#include "../expander/expander.h"
#include "../string_list/string_list.h"
#include "includes/ast.h"
#include "includes/ast_evaluate.h"
#include "sys/types.h"
#include "sys/wait.h"

const ast_eval_func op_eval[] = { [AST_COMMAND] = ast_eval_command,
                                  [AST_COMMAND_LIST] = ast_eval_command_list,
                                  [AST_RULE_IF] = ast_eval_rule_if,
                                  [AST_COMPOUND_LIST] = ast_eval_compound_list,
                                  [AST_SIMPLE_COMMAND] =
                                      ast_eval_simple_command,
                                  [AST_REDIRECTION] = ast_eval_redirection,
                                  [AST_PIPELINE] = ast_eval_pipeline,
                                  [AST_LOOP] = ast_eval_loop,
                                  [AST_AND_OR] = ast_eval_and_or,
                                  [AST_NEGATION] = ast_eval_negation,
                                  [AST_FOR] = ast_eval_for,
                                  [AST_FUNCDEC] = ast_eval_funcdec,
                                  [AST_CASE] = ast_eval_case };

int ast_eval_pipeline(struct ast_base *ast)
{
    struct ast_pipeline *pipe_command = (struct ast_pipeline *)ast;
    if (pipe_command->left != NULL && pipe_command->right == NULL)
        return ast_eval(pipe_command->left);
    int pipefds[2];
    if (pipe(pipefds) == -1)
        return 1;

    pid_t pid_1 = fork();
    if (pid_1 == 0) // in child process
    {
        close(pipefds[0]);
        if (dup2(pipefds[1], STDOUT_FILENO) == -1)
            _exit(127);
        ast_eval(pipe_command->left);
        close(pipefds[1]);
        _exit(0);
    }

    pid_t pid_2 = fork();
    if (pid_2 == 0)
    {
        close(pipefds[1]);
        if (dup2(pipefds[0], STDIN_FILENO) == -1)
            _exit(127); // dup failed

        int ret = ast_eval(pipe_command->right);
        close(pipefds[0]);
        _exit(ret);
    }

    close(pipefds[0]);
    close(pipefds[1]);
    int status_1 = 0;
    int status_2 = 0;
    waitpid(pid_1, &status_1, 0);
    waitpid(pid_2, &status_2, 0);
    return WEXITSTATUS(status_2);
}

int ast_eval_funcdec(struct ast_base *ast)
{
    assigner_add_func(ast);
    return 0;
}

int ast_eval_rule_if(struct ast_base *ast)
{
    struct ast_rule_if *rule_if = (struct ast_rule_if *)ast;
    int rule_if_condition = op_eval[rule_if->left->type](rule_if->middle);
    if (rule_if_condition == 0)
        return op_eval[rule_if->middle->type](rule_if->left);
    else
    {
        if (rule_if->right != NULL)
            return op_eval[rule_if->right->type](rule_if->right);
        return 0;
    }
}
int ast_eval_and_or(struct ast_base *ast)
{
    struct ast_and_or *tree = (struct ast_and_or *)ast;
    if (strcmp(tree->op_type, "&&") == 0)
    {
        if (ast_eval(tree->left) == 0)
        {
            if (ast_eval(tree->right) == 0)
                return 0;
            else
                return 1;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if (ast_eval(tree->left) == 0)
            return 0;
        else
        {
            if (ast_eval(tree->right) == 0)
                return 0;
            else
                return 1;
        }
    }
}

int ast_eval_negation(struct ast_base *ast)
{
    struct ast_negation *tree = (struct ast_negation *)ast;
    return !ast_eval(tree->to_negate);
}

int ast_eval(struct ast_base *tree)
{
    if (tree == NULL)
        return 0;

    return op_eval[tree->type](tree);
}
