#define _POSIX_C_SOURCE 200809L
#include "linked_list.h"

#include <string.h>
struct list *list_init(struct ast_base *value)
{
    struct list *res = calloc(1, sizeof(struct list));
    if (!res)
        return NULL;

    res->child = value;

    return res;
}

struct list *list_append(struct list *l, struct ast_base *child)
{
    if (l->child == NULL)
    {
        l->child = child;
        l->next = NULL;
        return l;
    }
    struct list *new = list_init(child);
    if (!new)
        return l;
    struct list *traverse = l;
    while (traverse->next != NULL)
    {
        traverse = traverse->next;
    }
    traverse->next = new;
    return l;
}

struct ast_base *list_get(struct list *l, char *fname)
{
    if (l == NULL)
        return NULL;
    struct list *traverse = l;
    while (traverse != NULL && strcmp(fname, traverse->func_name) != 0)
    {
        traverse = traverse->next;
    }
    return traverse == NULL ? NULL : traverse->child;
}

static struct list *list_append_func(struct list *l, struct ast_funcdec *func)
{
    if (l == NULL)
    {
        l = list_init(func->shell_command);
        l->func_name = strdup(func->fname);
        l->next = NULL;
        return l;
    }
    struct list *new = list_init(func->shell_command);
    new->func_name = strdup(func->fname);
    if (!new)
        return l;
    struct list *traverse = l;
    while (traverse->next != NULL)
    {
        traverse = traverse->next;
    }
    traverse->next = new;
    return l;
}
struct list *list_update_func(struct list *l, struct ast_base *data)
{
    struct ast_funcdec *tree = (struct ast_funcdec *)data;
    struct list *tree_to_update = l;
    while (tree_to_update != NULL
           && strcmp(tree->fname, tree_to_update->func_name) != 0)
    {
        tree_to_update = tree_to_update->next;
    }

    if (tree_to_update != NULL)
    {
        ast_free(tree_to_update->child);
        tree_to_update->child = data;
    }
    else
    {
        l = list_append_func(l, tree);
    }
    return l;
}

void list_free(struct list *l)
{
    if (!l)
        return;

    ast_free(l->child);
    list_free(l->next);

    free(l);
}

struct list *list_remove(struct list *l, char *key)
{
    if (!l)
        return l;
    if (strcmp(l->func_name, key) == 0)
    {
        struct list *to_delete = l;
        l = l->next;
        ast_free(to_delete->child);
        free(to_delete->func_name);
        free(to_delete);
    }
    else
    {
        struct list *cur = l->next;
        struct list *prev = l;
        while (cur)
        {
            if (strcmp(cur->func_name, key) == 0)
            {
                prev->next = cur->next;
                ast_free(cur->child);
                free(cur->func_name);
                free(cur);
                // FREE ALL THE THINGS HERE BAPTOU PLS
                return l;
            }
            else
            {
                prev = cur;
                cur = cur->next;
            }
        }
    }
    return l;
}
