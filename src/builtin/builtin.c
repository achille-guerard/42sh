#define _POSIX_C_SOURCE 200112L
#include "./builtin.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../assigner/assigner.h"
#include "../expander/expander.h"

int builtin_true(void)
{
    return 0;
}

int builtin_false(void)
{
    return 1;
}

static int builtin_echo_escape(char **str, int i, int j)
{
    if (!str[i][j + 1])
    {
        printf("\\");
        return j;
    }
    char c = str[i][j + 1];
    if (c == 't')
    {
        printf("\t");
        j++;
    }
    else if (c == 'n')
    {
        printf("\n");
        j++;
    }
    else if (c == '\n')
    {
        printf("\n");
        j++;
    }
    else if (c == '\\')
    {
        j++;
        printf("\\");
    }
    else
        printf("\\");
    return j;
}

// checks for the different arguments
// return the new index
static int manage_flags(char **str, int *escapes, int *newline)
{
    int i = 1;
    for (; str[i] != NULL; i++)
    {
        if (str[i][0] != '-')
            break;
        for (int j = 1; str[i][j]; j++)
        {
            if (str[i][j] == 'n')
            {
                *newline = 0;
            }
            else if (str[i][j] == 'e')
            {
                *escapes = 1;
            }
            else if (str[i][j] == 'E')
            {
                *escapes = 0;
            }
            else
            {
                return i;
            }
        }
    }
    return i;
}

int builtin_echo(char **str)
{
    if (!str || !str[0])
        return 1;
    if (!str[1])
    {
        printf("\n");
    }
    else
    {
        // we set up the pointers
        // associated to flags
        int val_e = 0;
        int val_n = 1;
        int *escapes = &val_e;
        int *newline = &val_n;
        int first_line = 1;

        // check flags
        int i = manage_flags(str, escapes, newline);
        // we iterate over the sequence of word
        for (; str[i]; i++)
        {
            // print a space between each words
            if (first_line == 0)
                printf(" ");
            // we iterate over the word
            for (int j = 0; str[i][j]; j++)
            {
                if (*escapes && str[i][j] == '\\')
                {
                    j = builtin_echo_escape(str, i, j);
                }
                else
                    putchar(str[i][j]);
            }
            first_line = 0;
        }
        if (*newline)
            printf("\n");
    }
    return 0;
}

// str[0] should be exit
int builtin_exit(char **str)
{
    if (str[1] != NULL && str[2] != NULL)
    {
        errx(1, "42sh: Too many arguments");
    }
    if (!str[1])
    {
        char *last_exit_code = assigner_get_shell_var("?");
        // exit value of the last command executed
        if (last_exit_code != NULL)
        {
            exit(atoi(last_exit_code));
        }
        else // zero if no command was executed
        {
            exit(0);
        }
    }
    else
    {
        exit(atoi(str[1]));
    }
}

static char is_number(char *str)
{
    size_t i = 0;
    while (str[i] != 0)
    {
        if (!isdigit(str[i]))
            return 0;
        i++;
    }
    return 1;
}

int builtin_break(char **str, struct boolean *boolean)
{
    if (str[1] != 0)
    {
        if (!is_number(str[1]))
            return 1;

        (*boolean).br = atoi(str[1]);
        (*boolean).is_br_set = 1;
    }
    else if (str[1] == 0)
    {
        (*boolean).br = 1;
        (*boolean).is_br_set = 1;
    }
    else
        return 1;
    return 0;
    // if HOME not defined, it is an
    // undefined behavior by the SCL
}

int builtin_continue(char **str, struct boolean *boolean)
{
    if (str[1] != 0)
    {
        if (!is_number(str[1]))
            return 1;

        (*boolean).con = atoi(str[1]);
        (*boolean).is_con_set = 1;
    }
    else if (str[1] == 0)
    {
        (*boolean).con = 1;
        (*boolean).is_con_set = 1;
    }
    else
        return 1;
    return 0;
}

static int export_check_format(char *str)
{
    int i = 0;
    if (str[i] == 0)
        return 0;
    while (str[i] != 0 && valid_naming(str[i]))
    {
        i++;
    }
    if (str[i] == 0)
    {
        return 1; // format: export name
    }
    if (str[i] != '=' && !valid_naming(str[i]))
    {
        return 0;
    }
    if (str[i] == '=')
    {
        i++;
        while (str[i] != 0 && valid_naming(str[i]))
        {
            i++;
        }
        if (str[i] == 0)
        {
            return 2; // format: export name=value
        }
        else
        {
            return 0; // wrong format
        }
    }
    else
    {
        return 0;
    }
}

int builtin_export(char **str)
{
    // split assignation in two tokens and set env variable
    int status = export_check_format(str[1]);
    char *key = strtok(str[1], "=");
    char *val = strtok(NULL, "=");
    if (status == 1) // format: export name
    {
        char *shell_value = assigner_get_shell_var(key);
        if (shell_value != NULL)
        {
            // remove var from shell_variables
            setenv(key, shell_value, 1);
        }
        return 0;
    }
    else if (status == 2) // format: export name=value
    {
        // remove variable from shell_variables
        if (val != NULL)
            setenv(key, val, 1);
        return 0;
    }
    else
    {
        return 1;
    }
}

static int unset_manage_flags(char **str, int *function, int *variable)
{
    int i = 1;
    for (; str[i] != NULL; i++)
    {
        if (str[i][0] != '-')
            break;
        for (int j = 1; str[i][j]; j++)
        {
            if (str[i][j] == 'v')
            {
                // unset variable
                *variable = 1;
            }
            else if (str[i][j] == 'f')
            {
                // unset function
                *function = 1;
            }
            else
            {
                return i; // flag not recognized so consider it a variable
            }
        }
    }
    return i;
}

int builtin_unset(char **str)
{
    if (!str || !str[0])
        return 0;
    int fun = 0;
    int var = 0;
    int *function = &fun;
    int *variable = &var;
    int i = unset_manage_flags(str, function, variable);
    for (; str[i]; i++)
    {
        // print a space between each words
        if (*function == 1)
        {
            // unset function
            assigner_remove_func(str[i]);
        }
        if (*variable == 1)
        {
            // unset variable
            // assigner_shell_remove_var(str[i]);
            unsetenv(str[i]);
        }
        else
        {
            if (getenv(str[i]) != NULL)
                unsetenv(str[i]);
            else
                assigner_remove_func(str[i]);
        }
    }
    return 0;
}
