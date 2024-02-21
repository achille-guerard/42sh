#ifndef AST_EVALUATE_H
#define AST_EVALUATE_H

#include "ast.h"

struct ast_base;
int ast_eval_command(struct ast_base *ast);
int ast_eval_simple_command(struct ast_base *ast);
int ast_eval_rule_if(struct ast_base *ast);
int ast_eval_command_list(struct ast_base *ast);
int ast_eval_compound_list(struct ast_base *ast);
int ast_eval_redirection(struct ast_base *ast);
int ast_eval_pipeline(struct ast_base *ast);
int ast_eval_negation(struct ast_base *ast);
int ast_eval_and_or(struct ast_base *ast);
int ast_eval_loop(struct ast_base *ast);
int ast_eval_for(struct ast_base *ast);
int ast_eval_funcdec(struct ast_base *ast);
int ast_eval_case(struct ast_base *ast);
int ast_eval_case_item(struct ast_base *ast);
int ast_eval(struct ast_base *tree);

typedef int (*const ast_eval_func)(struct ast_base *ast);

extern const ast_eval_func op_eval[];

#endif /* !AST_EVALUATE_H */
