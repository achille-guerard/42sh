#ifndef BUILTIN_H
#define BUILTIN_H
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lexer/token.h"

struct boolean
{
    size_t br;
    char is_br_set;
    size_t con;
    char is_con_set;
    size_t max_nest;
};

int builtin_true(void);

int builtin_false(void);

int builtin_echo(char **str);

int builtin_exit(char **str);

int builtin_break(char **str, struct boolean *boolean);

int builtin_continue(char **str, struct boolean *boolean);

int builtin_cd(char **str);

int builtin_export(char **str);

int builtin_unset(char **str);

int builtin_dot(char **str);

#endif /* BUILTIN_H */
