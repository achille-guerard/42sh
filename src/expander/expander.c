#define _POSIX_C_SOURCE 200809L
#include "expander.h"

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../assigner/assigner.h"
#include "../parser/parser.h"

// these are the char we can escape in DOUBLE QUOTE mode
static int backslashable(char c)
{
    if (c == '\\' || c == '`' || c == '\"' || c == '\n' || c == '$')
        return 1;
    else
        return 0;
}

static int char_is_quoting(char a)
{
    if (a == '\'' || a == '\"' || a == '\\')
        return 1;
    else
        return 0;
}

// FOR THE EXPANSION OF VAR AND TILDE
static void update_status(enum quoted *status, char c)
{
    int entered = 0;
    if (c == '\\')
    {
        *status = BACKSLASHED;
        entered = 1;
    }
    if (c == '\'')
    {
        if (*status == SINGLE_QUOTED)
            *status = FREE;
        else
            *status = SINGLE_QUOTED;
    }
    if (c == '\"')
    {
        if (*status == DOUBLE_QUOTED)
            *status = FREE;
        else
            *status = DOUBLE_QUOTED;
    }
    if (*status == BACKSLASHED && !entered)
    {
        *status = FREE;
    }
}

// FOR THE QUOTE REMOVAL
static int handle_status(enum quoted *status, char *text, int *i, int len)
{
    char c = text[*i];
    // if we are at the end we dont care about the status

    if (c == '\'')
    {
        if (*status == FREE)
        {
            *status = SINGLE_QUOTED;
            (*i)++;
        }
        else if (*status == SINGLE_QUOTED)
        {
            *status = FREE;
            (*i)++;
        }
        // if its DOUBLE_QUOTED we dont change status and we dont skip the
        // single
    }
    else if (c == '\"')
    {
        if (*status == FREE)
        {
            *status = DOUBLE_QUOTED;
            (*i)++;
        }
        else if (*status == DOUBLE_QUOTED)
        {
            *status = FREE;
            (*i)++;
        }
        // same here we dont change status and dont skip the double quote
    }
    else if (c == '\\')
    {
        if (*status == FREE)
            (*i)++;

        if (*status == DOUBLE_QUOTED && *i < len
            && backslashable(text[(*i) + 1]))
            (*i)++;
        return 1;
    }
    return 0;
}

int valid_naming(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else if (c == '_')
        return 1;
    else
        return 0;
}

static int is_special(char c)
{
    if (c == '$' || c == '?' || c == '@' || c == '#' || c == '*')
        return 1;
    else if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

static int is_dolla(char c)
{
    if (c == '$' || c == '`')
        return 1;
    else
        return 0;
}

static int is_closing(char c)
{
    if (c == ')' || c == '`')
        return 1;
    else
        return 0;
}

static int is_opening(char c)
{
    if (c == '(' || c == '`')
        return 1;
    else
        return 0;
}

// allocates the imput and returns it
//  or on error return a static NULL
static char *get_subcommand(char *text, int *i)
{
    char *input = calloc(1000, sizeof(char));
    int j = 0; // index in input
    int rec_nb = 1;
    while (rec_nb != 0 && text[*i] != EOF)
    {
        if (is_closing(text[*i]))
            rec_nb--;

        else if (is_opening(text[*i]))
            rec_nb++;
        if (rec_nb != 0)
            input[j++] = text[(*i)++];
    }
    if (j == 0)
    {
        free(input);
        return NULL;
    }
    return input;
}

static char *command_subing(char *text, int *i)
{
    char *input = get_subcommand(text, i);
    char *res = calloc(1000, sizeof(char));
    if (input == NULL)
        return res;

    if (text[*i] == EOF)
        errx(1, "bad substitution, did not match any closing parenthesis");

    // create the pipe
    int fds[2];
    pipe(fds);

    // they see me fokiiinn they chasiiiiin
    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);
        int exitcode = parser_loop(IO_STRING, input, PARSER_OK);
        _exit(exitcode);
    }
    // back in the parent
    close(fds[1]);

    FILE *output = fdopen(fds[0], "r");
    char r;
    int k = 0; // index in res

    while ((r = fgetc(output)) != -1)
    {
        res[k++] = r;
    }
    res[k--] = '\0';

    while (res[k] == '\n')
        res[k--] = '\0';

    close(fds[0]);
    int status = 0;
    waitpid(pid, &status, 0);

    free(input);
    return res;
}

// extracts the key from text and store it in key updates i
static void extract_key(char *text, char *key, int *i)
{
    int j = 0; // index in res

    if (is_special(text[*i]))
    {
        key[j++] = text[(*i)++];
        key[j] = '\0';
    }
    else
    {
        while (valid_naming(text[*i]))
            key[j++] = text[(*i)++];
        key[j] = '\0';
    }
}

static char *replace_by_value(char *res, char *value, int *j, int len)
{
    // we need more space if value is longer than key
    res = realloc(res, len + strlen(value) + 10);

    int i = 0;
    while (value[i] != '\0')
        res[(*j)++] = value[i++];
    return res;
}

static char *expand_variables(char *text, int len)
{
    char *res = calloc(len + 1, sizeof(char));
    char *key = calloc(len + 1, sizeof(char));
    char *value;
    enum quoted status = FREE;
    int i = 0; // index of text
    int j = 0; // index of res
    while (text[i] != '\0')
    {
        update_status(&status, text[i]);
        if (!is_dolla(text[i]) || status == SINGLE_QUOTED) // if its not a dolla
            res[j++] = text[i++]; // we eat
        else if (i > 0 && text[i - 1] == '\\') // if the dolla is backslashed
            res[j++] = text[i++]; // we eat
        else
        {
            int backquoted = (text[i] == '`') ? 1 : 0;
            // 1st pass the $, ` and {, ( if it exists
            int braqueted = (text[++i] == '{') ? 1 : 0;
            int parenthesised = (text[i] == '(') ? 1 : 0;
            if (braqueted || parenthesised)
                i++;

            if (parenthesised || backquoted)
            {
                // command subing allocates the value hence dont forget to free
                value = command_subing(text, &i);
                i++; // pass the closing par ')' or '`'
                if (value)
                    res = replace_by_value(res, value, &j, len);
                free(value);
            }
            else
            {
                // we extract the key
                extract_key(text, key, &i);
                // we get the value corresponding to our key
                // Here the value is on the stack not on the heap
                value = assigner_get_shell_var(key);
                if (!value)
                    value = assigner_get_env_var(key);

                // 3rd we skip the }
                if (braqueted)
                    i++;
                if (value)
                    res = replace_by_value(res, value, &j, len);
            }
        }
    }
    res[j] = '\0';
    free(key);
    free(text);
    return res;
}

static char *quote_removal(char *text, int len)
{
    enum quoted status = FREE;
    char *res = calloc(len + 1, sizeof(char));
    int i = 0; // index of text
    int j = 0; // index of res
    int backslashed = 0;
    while (text[i] != '\0')
    {
        backslashed = handle_status(&status, text, &i, len);

        if (text[i] == '\'' && status == DOUBLE_QUOTED)
            res[j++] = text[i++];

        if (backslashed || (i < len && !char_is_quoting(text[i])))
            res[j++] = text[i++];
    }
    // terminate the string pleeeeease
    res[j] = '\0';
    free(text);
    if (status == SINGLE_QUOTED || status == DOUBLE_QUOTED)
    {
        free(res);
        errx(2, "quote unmatched in expansion");
    }
    return res;
}

static char *tilde(char *text, int len)
{
    enum quoted status = FREE;
    char *res = calloc(len + 1, sizeof(char));
    int i = 0; // index of text
    int j = 0; // index of res
    char *value;
    while (text[i] != '\0')
    {
        update_status(&status, text[i]);
        if (status == FREE && text[i] == '~')
        {
            value = getenv("HOME");
            res = replace_by_value(res, value, &j, len);
            i++;
        }
        else
            res[j++] = text[i++];
    }
    res[j] = '\0';
    free(text);
    return res;
}

char *string_expander(char *text, int len)
{
    text = tilde(text, len);
    len = strlen(text); // the len might have changed
    text = expand_variables(text, len);
    len = strlen(text); // the len might have changed
    text = quote_removal(text, len);

    // improve this by making this function void
    return text;
}
