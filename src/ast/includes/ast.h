#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "../../linked_list/linked_list.h"
#include "./ast_create.h"
#include "./ast_evaluate.h"
#include "./ast_free.h"

enum ast_type
{
    AST_BASE,
    AST_COMMAND,
    AST_SIMPLE_COMMAND,
    AST_COMMAND_LIST,
    AST_COMPOUND_LIST,
    AST_RULE_IF,
    AST_NEGATION,
    AST_PIPELINE,
    AST_REDIRECTION,
    AST_AND_OR,
    AST_LOOP,
    AST_FOR,
    AST_FUNCDEC,
    AST_CASE,
    AST_CASE_ITEM
};

enum loop_type
{
    UNTIL,
    WHILE
};

struct ast_base
{
    enum ast_type type;
};

struct ast_command
{
    struct ast_base base;
    struct ast_base *child;
    struct list *redirections; // list of redirections
};

struct ast_simple_command
{
    struct ast_base base;
    struct string_list *commands;
    struct string_list *expanded;
    struct string_list *assignation;
    struct list *redirections; // list of redirections
};

struct ast_rule_if
{
    struct ast_base base;
    struct ast_base *left; // condition
    struct ast_base *middle; // then
    struct ast_base *right; // else or elif
};

struct ast_command_list
{
    struct ast_base base;
    struct list *commands;
};

struct ast_compound_list
{
    struct ast_base base;
    char is_ss;
    struct list *commands;
};

struct ast_redirection
{
    struct ast_base base;
    int io_number;
    char *redir_type;
    char *destination; // destination of the redirection
    int fd_backup;
    int fd_to_restore;
    int fd_open; // file opened
};

struct ast_pipeline
{
    struct ast_base base;
    struct ast_base *left; // left command
    struct ast_base *right; // right command
};

struct ast_negation
{
    struct ast_base base;
    struct ast_base *to_negate;
};

struct ast_and_or
{
    struct ast_base base;
    struct ast_base *left;
    struct ast_base *right;
    char *op_type;
};

struct ast_loop
{
    struct ast_base base;
    struct ast_base *condition;
    struct ast_base *instruction;
    enum loop_type ltype;
};

struct ast_for
{
    struct ast_base base;
    char *name; // for name
    char **words; // in words
    size_t size;
    size_t capacity;
    struct ast_base *instruction; // do instructions
};

struct ast_funcdec
{
    struct ast_base base;
    char *fname;
    struct ast_base *shell_command;
};

struct ast_case
{
    struct ast_base base;
    struct list *items;
    char *pattern;
};

struct ast_case_item
{
    struct ast_base base;
    struct string_list *patterns;
    struct ast_base *instruction;
};

#endif /* !AST_H */
