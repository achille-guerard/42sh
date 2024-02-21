#include "../assigner/assigner.h"
#include "../parser/parser.h"
#include "./builtin.h"

// check if $PATH can be a file
int builtin_dot(char **str)
{
    if (str[1] == NULL)
        errx(2,
             "42sh: .: filename argument required\n.: usage: . filename "
             "[arguments]");
    else
    {
        char *dir = calloc(4096, 1);
        if (strncmp(str[1], "./", 2) != 0)
        {
            strcat(dir, getenv("PATH"));
            int len = strlen(dir);
            if (dir[len - 1] != '/')
            {
                strcat(dir, "/");
                len++;
            }
        }
        strcat(dir, str[1]);
        int ret = parser_loop(IO_FILE, dir, PARSER_OK);
        free(dir);
        return ret;
    }
}
