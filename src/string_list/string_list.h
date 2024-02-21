#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct string_list
{
    char *key;
    char *value;
    struct string_list *next;
};

struct string_list *string_list_init(char *key, char *value);
size_t string_list_len(struct string_list *l);
struct string_list *string_list_append(struct string_list *l, char *key,
                                       char *value);
struct string_list *string_list_insert_at(struct string_list *l,
                                          struct string_list *to_insert,
                                          size_t position);
struct string_list *string_list_pop(struct string_list *l);
struct string_list *string_list_remove_key(struct string_list *l, char *key);
char *string_list_get_value(struct string_list *l, char *key);
struct string_list *string_list_update_value(struct string_list *l, char *key,
                                             char *new_val);
void string_list_free(struct string_list *l);

#endif /* !STRING_LIST_H */
