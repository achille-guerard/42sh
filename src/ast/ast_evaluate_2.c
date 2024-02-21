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

struct boolean boolean = { 0, 0, 0, 0, 0 };

static void perfom_redirection(struct ast_redirection *as, int destination,
                               int to_restore)
{
    if (as->io_number != -1)
        to_restore = as->io_number;
    as->fd_open = destination;
    as->fd_backup = dup(to_restore);
    // redirect to_restore to destination of the redirection
    // i.e. the file we opened in eval_redir
    dup2(destination, to_restore);
    as->fd_to_restore = to_restore;
}

int ast_eval_redirection(struct ast_base *ast)
{
    struct ast_redirection *as = (struct ast_redirection *)ast;
    // Note : > && >| for 42sh have the same execution
    int fd = -1;
    if (strcmp(as->redir_type, ">") == 0 || strcmp(as->redir_type, ">|") == 0)
    {
        fd = open(as->destination, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        perfom_redirection(as, fd, STDOUT_FILENO);
    }
    else if (strcmp(as->redir_type, ">>") == 0)
    {
        fd = open(as->destination, O_CREAT | O_WRONLY | O_APPEND, 0644);
        perfom_redirection(as, fd, STDOUT_FILENO);
    }
    else if (strcmp(as->redir_type, "<") == 0)
    {
        fd = open(as->destination, O_RDONLY, 0644);
        if (fd == -1)
            errx(1, "%s: No such file or directory", as->destination);
        perfom_redirection(as, fd, STDIN_FILENO);
    }
    else if (strcmp(as->redir_type, ">&") == 0)
    {
        fd = atoi(as->destination);
        // check if the fd exists
        if (fcntl(fd, F_GETFD) == -1)
            errx(1, "%s: Bad file descriptor", as->destination);
        perfom_redirection(as, fd, STDOUT_FILENO);
    }
    else if (strcmp(as->redir_type, "<&") == 0)
    {
        fd = atoi(as->destination);
        if (fcntl(fd, F_GETFD) == -1)
            errx(1, "%s: Bad file descriptor", as->destination);
        perfom_redirection(as, fd, STDIN_FILENO);
    }
    // "<>" if the process is perfect
    else
    {
        fd = open(as->destination, O_RDWR | O_CREAT, 0644);
        perfom_redirection(as, fd, STDIN_FILENO);
    }
    return 0;
}

static int expand_all(struct ast_simple_command *tree)
{
    // if we dont have any command, then the assignment will be global
    int is_global = (tree->commands->value == NULL) ? 1 : 0;
    char *expanded = NULL;
    /// we traverse first the commands
    struct string_list *traverse = tree->commands;
    while (traverse && traverse->value)
    {
        // printf("before : %s\n", traverse_c->value);
        expanded =
            string_expander(strdup(traverse->value), strlen(traverse->value));
        tree->expanded = string_list_append(tree->expanded, NULL, expanded);
        // printf("after : %s\n\n", traverse_c->value);

        // checking for expansion error
        if (traverse->value == NULL)
            return 1;
        traverse = traverse->next;
    }

    // we traverse the assignations
    traverse = tree->assignation;
    while (traverse && traverse->value)
    {
        traverse->value =
            string_expander(traverse->value, strlen(traverse->value));

        if (traverse->value == NULL)
            return 1;

        // assign the variables
        assigner_string_extractor(is_global, traverse->value);

        traverse = traverse->next;
    }
    return 0;
}

static void remove_env(struct ast_simple_command *tree)
{
    struct string_list *traverse = tree->assignation;
    while (traverse && traverse->value)
    {
        assigner_remove_env_var(traverse->value);
        traverse = traverse->next;
    }
}

static void manage_redirs(struct ast_simple_command *ast)
{
    if (ast->redirections->child == NULL)
        return;
    struct list *redir = ast->redirections;
    while (redir)
    {
        ast_eval_redirection(redir->child);
        redir = redir->next;
    }
}

static void manage_redirs_command(struct ast_command *ast)
{
    if (ast->redirections->child == NULL)
        return;
    struct list *redir = ast->redirections;
    while (redir)
    {
        ast_eval_redirection(redir->child);
        redir = redir->next;
    }
}

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
int ast_eval_command(struct ast_base *ast)
{
    struct ast_command *tree = (struct ast_command *)ast;
    manage_redirs_command(tree);
    int ret = ast_eval(tree->child);
    manage_redirs_free(tree->redirections);
    return ret;
}

static void string_list_cpy(char **commands, struct string_list *l)
{
    struct string_list *traverse = l;
    size_t i = 0;
    while (traverse != NULL)
    {
        commands[i] = traverse->value;
        traverse = traverse->next;
        i++;
    }
}

/* This function handles $? */
static void add_exit_code(int value_to_convert)
{
    char *exit_code = calloc(10, sizeof(char));
    snprintf(exit_code, 10, "%i", value_to_convert);
    assigner_add_shell_var(strdup("?"), exit_code);
}

static int ast_manage_builtin_code(char **final_command,
                                   struct ast_simple_command *command)
{
    int ret = -1;
    if (strcmp(final_command[0], "echo") == 0)
        ret = builtin_echo(final_command);
    else if (strcmp(final_command[0], "true") == 0)
        ret = builtin_true();

    else if (strcmp(final_command[0], "false") == 0)
        ret = builtin_false();
    else if (strcmp(final_command[0], "cd") == 0)
        ret = builtin_cd(final_command);
    else if (strcmp(final_command[0], "exit") == 0)
        ret = builtin_exit(final_command);
    else if (strcmp(final_command[0], "break") == 0)
        ret = builtin_break(final_command, &boolean);
    else if (strcmp(final_command[0], "continue") == 0)
        ret = builtin_continue(final_command, &boolean);
    else if (strcmp(final_command[0], "export") == 0)
        ret = builtin_export(final_command);
    else if (strcmp(final_command[0], "unset") == 0)
        ret = builtin_unset(final_command);
    else if (strcmp(final_command[0], ".") == 0)
        ret = builtin_dot(final_command);
    if (ret != -1)
    {
        fflush(stdout);
        // if (command != NULL)
        //    manage_redirs_free(command->redirections);
        remove_env(command);
        string_list_free(command->expanded);
        command->expanded = NULL;
        free(final_command);
        add_exit_code(ret);
        return ret;
    }
    return ret;
}

int ast_eval_simple_command(struct ast_base *ast)
{
    struct ast_simple_command *command = (struct ast_simple_command *)ast;
    expand_all(command);
    manage_redirs(command);
    size_t len = string_list_len(command->expanded);
    char **final_command = calloc(len + 1, sizeof(char *));
    string_list_cpy(final_command, command->expanded);
    // if no commands we just exit the evaluation
    if (!final_command[0])
    {
        free(final_command);
        return 0;
    }

    int ret = ast_manage_builtin_code(final_command, command);
    if (ret != -1)
    {
        return ret;
    }
    else
    {
        struct ast_base *tree = assigner_get_func(final_command[0]);
        if (tree != NULL)
        {
            free(final_command);
            return ast_eval(tree);
        }
        pid_t pid = fork();
        // in the child process
        if (pid == 0)
        {
            execvp(final_command[0], final_command);
            fprintf(stderr, "%s: command not found\n", final_command[0]);
            _exit(127);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            // manage_redirs_free(command->redirections);
            free(final_command);
            string_list_free(command->expanded);
            command->expanded = NULL;
            add_exit_code(WEXITSTATUS(status));
            return WEXITSTATUS(status);
        }
    }
    // fds closed in ast free function
    // manage_redirs_free(command->redirections);
    remove_env(command);
    free(final_command);
    add_exit_code(0);
    return 0;
}

int ast_eval_command_list(struct ast_base *ast)
{
    struct ast_command_list *command_list = (struct ast_command_list *)ast;
    struct list *traverse = command_list->commands;
    int return_value = 0;
    while (traverse != NULL && !boolean.is_con_set && boolean.br == 0)
    {
        return_value = op_eval[traverse->child->type](traverse->child);
        traverse = traverse->next;
    }
    if (boolean.con == 1)
        boolean.is_con_set = 0;
    if (boolean.max_nest == 1 && boolean.con > 1)
        boolean.is_con_set = 0;
    return return_value;
}

int ast_eval_compound_list(struct ast_base *ast)
{
    struct ast_compound_list *command_list = (struct ast_compound_list *)ast;
    if (command_list->is_ss)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            struct list *traverse = command_list->commands;
            int return_value = 0;
            while (traverse != NULL && !boolean.is_con_set && boolean.br == 0)
            {
                return_value = op_eval[traverse->child->type](traverse->child);
                traverse = traverse->next;
            }
            if (boolean.con == 1)
                boolean.is_con_set = 0;
            if (boolean.max_nest == 1 && boolean.con > 1)
                boolean.is_con_set = 0;
            _exit(return_value);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            // manage_redirs_free(command->redirections);
            return WEXITSTATUS(status);
        }
    }
    else
    {
        struct list *traverse = command_list->commands;
        int return_value = 0;
        while (traverse != NULL && !boolean.is_con_set && boolean.br == 0)
        {
            return_value = op_eval[traverse->child->type](traverse->child);
            traverse = traverse->next;
        }
        if (boolean.con == 1)
            boolean.is_con_set = 0;
        if (boolean.max_nest == 1 && boolean.con > 1)
            boolean.is_con_set = 0;
        return return_value;
    }
    /*
    struct ast_compound_list *command_list = (struct ast_compound_list *)ast;
    struct list *traverse = command_list->commands;
    int return_value = 0;
    while (traverse != NULL && !boolean.is_con_set && boolean.br == 0)
    {
        return_value = op_eval[traverse->child->type](traverse->child);
        traverse = traverse->next;
    }
    if (boolean.con == 1)
        boolean.is_con_set = 0;
    if (boolean.max_nest == 1 && boolean.con > 1)
        boolean.is_con_set = 0;
    return return_value;*/
}

int ast_eval_loop(struct ast_base *ast)
{
    struct ast_loop *tree = (struct ast_loop *)ast;
    int ret = 0;
    boolean.max_nest++;
    if (tree->ltype == WHILE)
    {
        while (ast_eval(tree->condition) != 1 && boolean.br == 0
               && !boolean.is_con_set)
        {
            ret = ast_eval(tree->instruction);
        }
    }

    else
    {
        while (ast_eval(tree->condition) != 0 && boolean.br == 0
               && !boolean.is_con_set)
        {
            ret = ast_eval(tree->instruction);
        }
    }
    if (boolean.is_br_set)
        boolean.br -= 1;
    if (boolean.con != 0)
        boolean.con -= 1;
    if (boolean.con == 1)
        boolean.is_con_set = 1;
    boolean.max_nest--;
    return ret;
}

int ast_eval_for(struct ast_base *ast)
{
    struct ast_for *tree = (struct ast_for *)ast;
    boolean.max_nest++;

    size_t i = 0;
    while (tree->words[i] != NULL)
    {
        assigner_add_shell_var(strdup(tree->name), strdup(tree->words[i]));
        if (!boolean.is_con_set && boolean.br == 0)
            ast_eval(tree->instruction);
        i++;
    }
    if (boolean.is_br_set)
        boolean.br -= 1;
    if (boolean.con != 0)
        boolean.con -= 1;
    if (boolean.con == 1)
        boolean.is_con_set = 1;
    boolean.max_nest--;
    return 0;
}
