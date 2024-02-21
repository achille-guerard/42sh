#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <unistd.h>

#include "../linked_list/linked_list.h"
#include "assigner.h"
#define PATH_MAX 4096
struct list *func_list = NULL;

void assigner_add_func(struct ast_base *ast)
{
    func_list = list_update_func(func_list, ast);
}

struct ast_base *assigner_get_func(char *fname)
{
    return list_get(func_list, fname);
}

// update "PWD" with the current pwd
void set_pwd(void)
{
    char *str1 = getenv("PWD");
    if (str1)
        return;
    char str[PATH_MAX];
    memset(str, 0, PATH_MAX);
    // get the current pwd
    setenv("PWD", getcwd(str, PATH_MAX), 1);
}

void assigner_remove_func(char *key)
{
    func_list = list_remove(func_list, key);
}
