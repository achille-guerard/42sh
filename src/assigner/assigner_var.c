// #define _POSIX_C_SOURCE 200112L
#include <stddef.h>
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../string_list/string_list.h"
#include "assigner.h"

struct string_list *shell_variables = NULL;

/*
 * @brief
 * appends new variable to glist with value
 * if variable name already exists, it updates its value
 */
void assigner_add_shell_var(char *key, char *value)
{
    shell_variables = string_list_update_value(shell_variables, key, value);
}
/*
 * @brief
 * appends new variable to env with value
 * if variable name already exists, it updates its value
 */
void assigner_add_env_var(char *key, char *value)
{
    setenv(key, value, 1);
}

void assigner_remove_env_var(char *key)
{
    unsetenv(key);
}

char *assigner_get_shell_var(char *key)
{
    return string_list_get_value(shell_variables, key);
}

char *assigner_get_env_var(char *key)
{
    return getenv(key);
}

/*
 * @brief
 * reads the string given in argument and adds the pair key value
 * into the the shell variables if is_global is 1
 * into the env_variables if is global is 0
 */
void assigner_string_extractor(int is_global, char *text)
{
    int i = 0; // index in text
    while (text[i] != '=')
    {
        i++;
    }
    int end_key = i; // mark of the ending of the key
    i++; // pass the equal sign

    char *value = calloc(strlen(text) + 1, sizeof(char));
    int j = 0; // index in value
    while (text[i] != '\0')
    {
        value[j++] = text[i++];
    }

    text[end_key] = '\0'; // removes the value part

    if (is_global) // set as shell_variable
    {
        if (getenv(text) != NULL)
        {
            // update variable in env
            setenv(text, value, 1);
        }
        else
        {
            // add variable in shell_variables
            assigner_add_shell_var(strdup(text), value); // strdup text here
        }
    }
    else
    {
        // char *var = assigner_get_shell_var(text);
        assigner_add_env_var(text, value);
        free(value);
    }
}

static size_t args_len(int argc, char **argv)
{
    size_t len = 0;
    for (int i = 2; i < argc; i++)
    {
        len += strlen(argv[i]);
    }
    return len;
}

static int get_digits(size_t size)
{
    int res = 0;
    while (size >= 10)
    {
        size = size / 10;
        res++;
    }
    return res + 1;
}

/* This function handles $*/
static void add_pid_code(void)
{
    char *exit_code = calloc(10, sizeof(char));
    snprintf(exit_code, 10, "%i", getpid());
    assigner_add_shell_var(strdup("$"), exit_code);
}

void init_shell_var(int argc, char **argv, int is_fd) // int argc, char **argv)
{
    add_pid_code();
    char *arobase = strdup("@");
    char *power = strdup("*");
    size_t total_len = args_len(argc, argv);
    if (is_fd)
    {
        char *args_var = calloc(total_len * 2, sizeof(char));
        for (int i = 2; i < argc; i++)
        {
            if (i > 2)
                strcat(args_var, " ");
            strcat(args_var, argv[i]);
        }
        // set $@
        assigner_add_shell_var(arobase, args_var);
        // set $*
        assigner_add_shell_var(power, args_var);
    }
    else
    {
        assigner_add_shell_var(arobase, NULL);
        assigner_add_shell_var(power, NULL);
    }

    // set $#
    int numb_digits = get_digits(total_len);
    char *nb_args = calloc(numb_digits + 1, sizeof(char));
    snprintf(nb_args, numb_digits + 1, "%zu", total_len);
    char *key_nb_args = strdup("#");
    assigner_add_shell_var(key_nb_args, nb_args);

    // set UID
    char *uid = calloc(10, sizeof(char));
    snprintf(uid, 10, "%i", getuid());
    char *key_uid = strdup("UID");
    assigner_add_shell_var(key_uid, uid);

    // set $?
    char *exit_code = calloc(10, sizeof(char));
    snprintf(exit_code, 10, "%i", 0);
    char *key_exit = strdup("?");
    assigner_add_shell_var(key_exit, exit_code);

    char *ifs_code = getenv("IFS");
    if (ifs_code == NULL)
    {
        setenv("IFS", " \t\n", 1);
    }
    set_pwd();
}
