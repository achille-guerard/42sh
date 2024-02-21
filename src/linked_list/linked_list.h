#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/includes/ast_free.h"

struct list
{
    struct ast_base *child;
    char *func_name;
    struct list *next;
};

struct list *list_init(struct ast_base *value);
struct list *list_append(struct list *l, struct ast_base *data);
struct ast_base *list_get(struct list *l, char *fname);
struct list *list_update_func(struct list *l, struct ast_base *data);
void list_free(struct list *l);
struct list *list_remove(struct list *l, char *key);

#endif /* !LINKED_LIST_H */
