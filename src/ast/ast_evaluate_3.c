#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../assigner/assigner.h"
#include "../builtin/builtin.h"
#include "../expander/expander.h"
#include "../string_list/string_list.h"
#include "includes/ast.h"
#include "includes/ast_evaluate.h"

int ast_eval_case(struct ast_base *ast)
{
    struct ast_case *tree = (struct ast_case *)ast;
    tree->pattern = string_expander(tree->pattern, strlen(tree->pattern));
    struct list *traverse = tree->items;
    tree->pattern = string_expander(tree->pattern, strlen(tree->pattern));
    while (traverse != NULL)
    {
        struct ast_case_item *item = (struct ast_case_item *)traverse->child;
        if (item != NULL)
        {
            struct string_list *traverse_str = item->patterns;
            while (traverse_str != NULL)
            {
                traverse_str->value = string_expander(
                    traverse_str->value, strlen(traverse_str->value));
                if (strcmp(traverse_str->value, tree->pattern) == 0)
                    return ast_eval(item->instruction);

                traverse_str = traverse_str->next;
            }
        }

        traverse = traverse->next;
    }

    return 0;
}
