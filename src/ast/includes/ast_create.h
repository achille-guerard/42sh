#ifndef AST_CREATE_H
#define AST_CREATE_H

#include "ast.h"

struct ast_base;
struct ast_command *ast_create_command(void);
struct ast_simple_command *ast_create_simple_command(void);
struct ast_rule_if *ast_create_rule_if(void);
struct ast_command_list *ast_create_command_list(void);
struct ast_redirection *ast_create_redirection(void);
struct ast_compound_list *ast_create_compound_list(void);
struct ast_pipeline *ast_create_pipeline(void);
struct ast_negation *ast_creation_negation(void);
struct ast_and_or *ast_create_and_or(void);
struct ast_loop *ast_create_loop(void);
struct ast_for *ast_create_for(void);
struct ast_funcdec *ast_create_funcdec(void);
struct ast_case *ast_create_case(void);
struct ast_case_item *ast_create_case_item(void);

#endif /* !AST_CREATE_H */
