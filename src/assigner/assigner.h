#ifndef ASSIGNER_H
#define ASSIGNER_H
#include "../ast/includes/ast.h"
void assigner_string_extractor(int is_global, char *text);
void assigner_add_shell_var(char *key, char *value);
void assigner_add_env_var(char *key, char *value);
void set_pwd(void);

char *assigner_get_shell_var(char *key);
char *assigner_get_env_var(char *key);
void assigner_remove_env_var(char *key);
void init_shell_var(int argc, char **argv, int is_fd); // int argc, char **argv)

void assigner_add_func(struct ast_base *ast);
struct ast_base *assigner_get_func(char *fname);
void assigner_remove_func(char *key);

#endif /* !ASSIGNER_H */
