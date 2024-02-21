#define _POSIX_C_SOURCE 200112L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../assigner/assigner.h"
#include "./builtin.h"
#include "stdlib.h"
#define PATH_MAX 4096
#include "../expander/expander.h"

static char *get_pwd(void)
{
    char *pwd = getenv("PWD");
    if (pwd != NULL)
    {
        return pwd;
    }
    set_pwd();
    char *pwd2 = getenv("PWD");
    return pwd2;
}

static int builtin_cd_no_args(void)
{
    char *c = getenv("HOME");
    if (c != NULL)
    {
        char *old_pwd = get_pwd();

        if (chdir(c) != -1)
        {
            setenv("OLDPWD", old_pwd, 1);
            set_pwd();
        }
        return 0;
    }
    fprintf(stderr, "cd: HOME not set\n");
    return 1;
    // if HOME not defined, it is an
    // undefined behavior by the SCL
}

static int delete_compo(char **new, size_t size)
{
    while (size > 0 && new[0][size] != '/')
    {
        new[0][size] = '\0';
        size--;
    }
    if (size > 0)
    {
        new[0][size] = '\0';
    }
    return size;
}

static char *cd_step7_8(char *str)
{
    // step 7
    size_t size = 0;
    char *new = calloc(PATH_MAX, 1);
    char *curpath = calloc(PATH_MAX, sizeof(char));
    if (str[0] != '/' && str[0] != '~')
    {
        char *pwd = get_pwd();
        size_t size_pwd = strlen(pwd);
        if (size_pwd + strlen(str) > 4096)
        {
            strcat(new, str);
            free(str);
            free(curpath);
            return new;
        }
        size = size_pwd;
        strcat(curpath, pwd);
        if (curpath[size - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, str);
    }
    else
    {
        strcat(curpath, str);
    }
    free(str);
    // step 8 convert to canonical form
    char *token = strtok(curpath, "/");
    size = 0;
    while (token != NULL)
    {
        // dot and slash should be deleted
        // slash is deleted by strtok
        if (strcmp(token, ".") != 0)
        {
            if (strcmp(token, "..") != 0)
            {
                strcat(new, "/");
                strcat(new, token);
                size += strlen(token) + 1;
            }
            else
            {
                size = delete_compo(&new, size);
            }
        }
        token = strtok(NULL, "/");
    }
    free(curpath);
    return new;
}

int builtin_cd(char **str)
{
    // step 1 & 2 : perfoms "cd" w/o arguments
    if (!str[1])
    {
        return builtin_cd_no_args();
    }
    else if (str[2]) // since str[1] exists
    {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }
    else if (strcmp(str[1], "-") == 0)
    {
        char *new_pwd = getenv("OLDPWD");
        char *initial_pwd = get_pwd();
        if (chdir(new_pwd) == -1)
        {
            fprintf(stderr, "42sh: cd : OLDPWD not set\n");
            return 1;
        }
        // we switch the old pwd with the new one
        setenv("PWD", new_pwd, 1);
        setenv("OLDPWD", initial_pwd, 1);
        printf("%s\n", new_pwd);
    }
    else
    {
        char *oldpwd = get_pwd();
        // curpath freed in step3 or end of step7_8
        char *curpath = calloc(PATH_MAX, sizeof(char));
        // step 3
        if (str[1][0] == '/')
        {
            if (!str[1][1])
            {
                free(curpath);
                if (chdir(str[1]) == -1)
                {
                    fprintf(stderr, "cd: %s: No such file or directory\n",
                            str[1]);
                    return 1;
                }
                set_pwd();
                setenv("OLDPWD", oldpwd, 1);
                return 0;
            }
            strcat(curpath, str[1]);
            // go to step 7
        }
        else // step 4 & 5
        {
            // step 6
            strcat(curpath, str[1]);
        }
        // step 7 & 8
        char *new = cd_step7_8(curpath);
        if (chdir(new) == -1)
        {
            fprintf(stderr, "cd: %s: No such file or directory\n", str[1]);
            free(new);
            return 1;
        }
        setenv("PWD", new, 1);
        setenv("OLDPWD", oldpwd, 1);
        free(new);
    }
    return 0;
}
