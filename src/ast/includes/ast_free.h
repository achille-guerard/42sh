#ifndef AST_FREE_H
#define AST_FREE_H

#include "ast.h"

struct ast_base;
void ast_free_command(struct ast_base *ast);
void ast_free_simple_command(struct ast_base *ast);
void ast_free_rule_if(struct ast_base *ast);
void ast_free_command_list(struct ast_base *ast);
void ast_free_compound_list(struct ast_base *ast);
void ast_free_redirection(struct ast_base *ast);
void ast_free_pipeline(struct ast_base *ast);
void ast_free_negation(struct ast_base *ast);
void ast_free_and_or(struct ast_base *ast);
void ast_free_loop(struct ast_base *ast);
void ast_free_for(struct ast_base *ast);
void ast_free_funcdec(struct ast_base *ast);
void ast_free_case(struct ast_base *ast);
void ast_free_case_item(struct ast_base *ast);
void ast_free(struct ast_base *tree);

typedef void (*const ast_free_func)(struct ast_base *ast);

extern const ast_free_func op_free[];

#endif /* !AST_FREE_H */
