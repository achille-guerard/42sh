#define _POSIX_C_SOURCE 200809L
#include "string_list.h"

#include <stddef.h>
#include <string.h>

struct string_list *string_list_init(char *key, char *value)
{
    struct string_list *res = calloc(1, sizeof(struct string_list));
    if (!res)
        return NULL;

    res->key = key;
    res->value = value;

    return res;
}

size_t string_list_len(struct string_list *l)
{
    size_t len = 0;
    struct string_list *traverse = l;
    while (traverse != NULL)
    {
        len++;
        traverse = traverse->next;
    }
    return len;
}

struct string_list *string_list_append(struct string_list *l, char *key,
                                       char *value)
{
    if (l == NULL)
    {
        l = string_list_init(key, value);
        return l;
    }
    if (l->key == NULL && l->value == NULL)
    {
        l->key = key;
        l->value = value;
        return l;
    }
    struct string_list *new = string_list_init(key, value);
    if (!new)
        return l;

    struct string_list *traverse = l;
    while (traverse->next != NULL)
    {
        traverse = traverse->next;
    }
    traverse->next = new;
    return l;
}
/*
struct string_list *string_list_insert_at(struct string_list *l,
                                          struct string_list *to_insert,
                                          size_t position)
{
    if (l == NULL)
    {
        return to_insert;
    }
    if (position == 0)
    {
        to_insert->next = l;
        return to_insert;
    }
    struct string_list *traverse = l;
    for (size_t i = 0; i < position - 1; i++)
    {
        if (traverse->next != NULL)
            traverse = traverse->next;
        else
        {
            traverse->next = to_insert;
            return l;
        }
    }
    if (traverse->next == NULL)
    {
        traverse->next = to_insert;
        return l;
    }
    to_insert->next = traverse->next;
    traverse->next = to_insert;
    return l;
}
*/
/*
struct string_list *string_list_remove_key(struct string_list *l, char *key)
{
    if (strcmp(l->key, key) == 0)
    {
        struct string_list *to_remove = l;
        l = l->next;
        to_remove->next = NULL;
        // add free of key and value when correctly allocated
         // if (traverse->key)
        //      free(traverse->key);
         // if (traverse->value)
         //      free(traverse->value);
        free(to_remove);
    }
    struct string_list *traverse = l;
    struct string_list *prev = l;
    while (traverse != NULL && (strcmp(traverse->key, key) != 0))
    {
        prev = traverse;
        traverse = traverse->next;
    }
    if (traverse == NULL)
    {
        return l;
    }
    else // it means that the key was found
    {
        // Will be added when we allocate correctly
        // if (traverse->key)
            free(traverse->key);
        if (traverse->value)
            free(traverse->value);
        prev->next = traverse->next;
        traverse->next = NULL;
        free(traverse);
        return l;
    }
}
*/
char *string_list_get_value(struct string_list *l, char *key)
{
    struct string_list *traverse = l;
    while (traverse != NULL && (strcmp(traverse->key, key) != 0))
    {
        traverse = traverse->next;
    }
    if (traverse != NULL && strcmp(traverse->key, key) == 0)
        return traverse->value;
    else
        return NULL;
}

struct string_list *string_list_update_value(struct string_list *l, char *key,
                                             char *new_val)
{
    struct string_list *traverse = l;
    while (traverse != NULL && (strcmp(traverse->key, key) != 0))
    {
        traverse = traverse->next;
    }
    if (traverse != NULL && strcmp(traverse->key, key) == 0)
    {
        // update the value if the key is found
        if (traverse->value)
        {
            free(traverse->key);
            free(traverse->value);
            traverse->value = new_val; // strdup(new_val); // strdup here
            traverse->key = key;
        }
    }
    else // key not found
    {
        // create new var
        return string_list_append(l, key, new_val);
    }
    return l;
}

void string_list_free(struct string_list *l)
{
    if (!l)
        return;

    if (l->value)
        free(l->value);
    if (l->key)
        free(l->key);
    string_list_free(l->next);

    free(l);
}
