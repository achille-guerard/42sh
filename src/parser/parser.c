#define _POSIX_C_SOURCE 200809L
#include "parser.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../linked_list/linked_list.h"
#include "../string_list/string_list.h"

static int input(struct ast_base **ast, struct lexer *lexer);
static int list(struct ast_base **ast, struct lexer *lexer);
static int and_or(struct ast_base **ast, struct lexer *lexer);
static int pipeline(struct ast_base **ast, struct lexer *lexer);
static int command(struct ast_base **ast, struct lexer *lexer);
static int simple_command(struct ast_base **ast, struct lexer *lexer);
static int element(struct ast_base **ast, struct lexer *lexer);
static int redirection(struct ast_base **ast, struct lexer *lexer);
static int redirection_command(struct ast_command **ast, struct lexer *lexer);
static int prefix(struct ast_base **ast, struct lexer *lexer);
static int shell_command(struct ast_base **ast, struct lexer *lexer);
static int rule_if(struct ast_base **ast, struct lexer *lexer);
static int rule_while(struct ast_base **ast, struct lexer *lexer);
static int rule_until(struct ast_base **ast, struct lexer *lexer);
static int rule_for(struct ast_base **ast, struct lexer *lexer);
static int rule_case(struct ast_base **ast, struct lexer *lexer);
static int case_clause(struct ast_base **ast, struct lexer *lexer);
static int case_item(struct ast_base **ast, struct lexer *lexer);
static int else_clause(struct ast_base **ast, struct lexer *lexer);
static int compound_list(struct ast_base **ast, struct lexer *lexer);
static int funcdec(struct ast_base **ast, struct lexer *lexer);
struct ast_base *parser_init(enum parser_status *status, struct lexer *lexer)
{
    // struct lexer *lexer = lexer_new(mode, file);

    struct ast_base *ast = NULL;
    enum parser_status stat = input(&ast, lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        errx(2, "Parse error");
    }

    *status = stat;
    return ast;
}

static int input(struct ast_base **ast, struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_EOF)
    {
        return PARSER_EOF;
    }

    if (lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        free(lexer_pop(lexer).text);
        return PARSER_NEWLINE;
    }

    enum parser_status stat = list(ast, lexer);

    if (stat == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;

    if (lexer_peek(lexer).type != TOKEN_EOF
        && lexer_peek(lexer).type != TOKEN_NEW_LINE)
    {
        // free ast
        ast_free(*ast);
        return PARSER_UNEXPECTED_TOKEN;
    }

    if (lexer_peek(lexer).type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        return PARSER_EOF;
    }

    // return ast
    // eat newline
    free(lexer_pop(lexer).text);
    return PARSER_OK;
}

static int list(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_command_list *tree = ast_create_command_list();

    enum parser_status stat = and_or(ast, lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    tree->commands = list_append(tree->commands, *ast);

    while (lexer_peek(lexer).type == TOKEN_SEMI)
    {
        // eat semi
        free(lexer_pop(lexer).text);

        if (lexer_peek(lexer).type < TOKEN_WORD
            && lexer_peek(lexer).type != TOKEN_LPAR
            && lexer_peek(lexer).type != TOKEN_LBRACE)
            break;

        stat = and_or(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            // free tree
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }

        tree->commands = list_append(tree->commands, *ast);
    }

    *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

static int and_or(struct ast_base **ast, struct lexer *lexer)
{
    enum parser_status stat = pipeline(ast, lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }

    struct ast_base *child = *ast;
    while (lexer_peek(lexer).type == TOKEN_OR_IF
           || lexer_peek(lexer).type == TOKEN_AND_IF)
    {
        struct ast_and_or *tree = ast_create_and_or();
        // eat operator
        tree->op_type = lexer_pop(lexer).text;
        tree->left = child;

        while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        {
            // eat newline
            free(lexer_pop(lexer).text);
        }

        stat = pipeline(&(tree->right), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }

        child = (struct ast_base *)tree;
    }
    *ast = (struct ast_base *)child;
    return PARSER_OK;
}

static int pipeline(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_pipeline *tree = ast_create_pipeline();
    char is_neg = (lexer_peek(lexer).type == TOKEN_BANG) ? 1 : 0;
    if (is_neg)
        free(lexer_pop(lexer).text);

    struct ast_pipeline *cp_tree = tree;
    enum parser_status stat = command(&(cp_tree->left), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    while (lexer_peek(lexer).type == TOKEN_PIPE)
    {
        // eat pipe
        struct ast_pipeline *child = ast_create_pipeline();
        free(lexer_pop(lexer).text);
        while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        {
            free(lexer_pop(lexer).text);
        }

        stat = command(&(cp_tree->right), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            ast_free((struct ast_base *)child);
            return PARSER_UNEXPECTED_TOKEN;
        }
        if (lexer_peek(lexer).type == TOKEN_PIPE)
        {
            child->left = cp_tree->right;
            cp_tree->right = (struct ast_base *)child;
            cp_tree = child;
        }
        else
        {
            ast_free((struct ast_base *)child);
        }
    }

    if (is_neg)
    {
        struct ast_negation *parent = ast_creation_negation();
        parent->to_negate = (struct ast_base *)tree;
        *ast = (struct ast_base *)parent;
    }
    else
        *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

static int command(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_command *tree = ast_create_command();

    enum token_type act = lexer_peek(lexer).type;
    if (lexer_peek(lexer).type == TOKEN_WORD && lexer->next_1.type == TOKEN_LPAR
        && lexer->next_2.type == TOKEN_RPAR)
    {
        enum parser_status stat = funcdec(&(tree->child), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }
        *ast = (struct ast_base *)tree;
        return PARSER_OK;
    }

    else if (act == TOKEN_IF || act == TOKEN_WHILE || act == TOKEN_UNTIL
             || act == TOKEN_FOR || act == TOKEN_LBRACE || act == TOKEN_LPAR
             || act == TOKEN_CASE)
    {
        enum parser_status stat = shell_command(&(tree->child), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }

        while ((lexer_peek(lexer).type >= TOKEN_LESS
                && lexer_peek(lexer).type <= TOKEN_LESSGREAT)
               || lexer_peek(lexer).type == TOKEN_IO_NUM)
        {
            stat = redirection_command(&tree, lexer);
            if (stat == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free((struct ast_base *)tree);
                return PARSER_UNEXPECTED_TOKEN;
            }
        }
        *ast = (struct ast_base *)tree;
        return PARSER_OK;
    }
    else if (lexer_peek(lexer).type == TOKEN_WORD
             || lexer_peek(lexer).type == TOKEN_IO_NUM
             || lexer_peek(lexer).type == TOKEN_ASSIGN
             || (lexer_peek(lexer).type >= TOKEN_LESS
                 && lexer_peek(lexer).type <= TOKEN_LESSGREAT))
    {
        enum parser_status stat = simple_command(&(tree->child), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }
        *ast = (struct ast_base *)tree;
        return PARSER_OK;
    }

    ast_free((struct ast_base *)tree);
    return PARSER_UNEXPECTED_TOKEN;
}

static int simple_command(struct ast_base **ast, struct lexer *lexer)
{
    char is_pref = 0;
    struct ast_simple_command *tree = ast_create_simple_command();
    while ((lexer_peek(lexer).type >= TOKEN_LESS
            && lexer_peek(lexer).type <= TOKEN_LESSGREAT)
           || lexer_peek(lexer).type == TOKEN_IO_NUM
           || lexer_peek(lexer).type == TOKEN_ASSIGN)
    {
        enum parser_status stat = prefix((struct ast_base **)&tree, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }
        is_pref = 1;
    }

    if (is_pref == 0 && lexer_peek(lexer).type != TOKEN_WORD)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    while (lexer_peek(lexer).type >= TOKEN_WORD
           || (lexer_peek(lexer).type >= TOKEN_LESS
               && lexer_peek(lexer).type <= TOKEN_LESSGREAT)
           || lexer_peek(lexer).type == TOKEN_RBRACE)
    {
        enum parser_status stat = element((struct ast_base **)&tree, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }
    }
    *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

static int funcdec(struct ast_base **ast, struct lexer *lexer)
{
    enum parser_status stat = PARSER_OK;

    struct ast_funcdec *tree = ast_create_funcdec();
    tree->fname = strdup(lexer_peek(lexer).text);
    free(lexer_pop(lexer).text); // eat word
    free(lexer_pop(lexer).text); // eat lpar
    free(lexer_pop(lexer).text); // eat rpar
    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        free(lexer_pop(lexer).text); // eat newline
    }
    struct ast_base *res_body = NULL;
    stat = shell_command(&res_body, lexer);

    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    tree->shell_command = res_body;
    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

static int element(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_simple_command *tree = (struct ast_simple_command *)*ast;
    if ((lexer_peek(lexer).type >= TOKEN_WORD
         && lexer_peek(lexer).type != TOKEN_IO_NUM)
        || lexer_peek(lexer).type == TOKEN_RBRACE)
    {
        tree->commands =
            string_list_append(tree->commands, NULL, lexer_pop(lexer).text);
    }
    else
    {
        enum parser_status stat = redirection(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
    }
    return PARSER_OK;
}

static int redirection(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_redirection *tree = ast_create_redirection();
    if (lexer_peek(lexer).type == TOKEN_IO_NUM)
    {
        tree->io_number = atoi(lexer_peek(lexer).text);
        free(lexer_pop(lexer).text);
    }

    tree->redir_type = lexer_pop(lexer).text;
    if (lexer_peek(lexer).type < TOKEN_WORD)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    tree->destination = lexer_pop(lexer).text;
    ((struct ast_simple_command *)*ast)->redirections =
        list_append(((struct ast_simple_command *)*ast)->redirections,
                    (struct ast_base *)tree);
    return PARSER_OK;
}

static int redirection_command(struct ast_command **ast, struct lexer *lexer)
{
    struct ast_redirection *tree = ast_create_redirection();
    if (lexer_peek(lexer).type == TOKEN_IO_NUM)
    {
        tree->io_number = atoi(lexer_peek(lexer).text);
        free(lexer_pop(lexer).text);
    }

    tree->redir_type = lexer_pop(lexer).text;
    if (lexer_peek(lexer).type < TOKEN_WORD)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    tree->destination = lexer_pop(lexer).text;
    (*ast)->redirections =
        list_append((*ast)->redirections, (struct ast_base *)tree);
    return PARSER_OK;
}

static int shell_command(struct ast_base **ast, struct lexer *lexer)
{
    enum parser_status stat = PARSER_OK;

    if (lexer_peek(lexer).type == TOKEN_IF)
        stat = rule_if(ast, lexer);

    else if (lexer_peek(lexer).type == TOKEN_WHILE)
        stat = rule_while(ast, lexer);

    else if (lexer_peek(lexer).type == TOKEN_UNTIL)
        stat = rule_until(ast, lexer);

    else if (lexer_peek(lexer).type == TOKEN_LBRACE)
    {
        free(lexer_pop(lexer).text);
        stat = compound_list(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN
            || lexer_peek(lexer).type != TOKEN_RBRACE)
            return PARSER_UNEXPECTED_TOKEN;
        free(lexer_pop(lexer).text);
    }

    else if (lexer_peek(lexer).type == TOKEN_LPAR)
    {
        free(lexer_pop(lexer).text);
        stat = compound_list(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN
            || lexer_peek(lexer).type != TOKEN_RPAR)
            return PARSER_UNEXPECTED_TOKEN;
        free(lexer_pop(lexer).text);
        ((struct ast_compound_list *)*ast)->is_ss = 1;
    }

    else if (lexer_peek(lexer).type == TOKEN_CASE)
        stat = rule_case(ast, lexer);

    else if (lexer_peek(lexer).type == TOKEN_FOR)
        stat = rule_for(ast, lexer);
    else
    {
        return PARSER_UNEXPECTED_TOKEN;
    }

    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }

    return PARSER_OK;
}

static int rule_while(struct ast_base **ast, struct lexer *lexer)
{
    // eat while
    free(lexer_pop(lexer).text);
    struct ast_loop *tree = ast_create_loop();
    tree->ltype = WHILE;

    enum parser_status stat = compound_list(&(tree->condition), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_DO)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // eat do
    free(lexer_pop(lexer).text);
    stat = compound_list(&(tree->instruction), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_DONE)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // eat done
    free(lexer_pop(lexer).text);

    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

static int rule_until(struct ast_base **ast, struct lexer *lexer)
{
    // eat until
    free(lexer_pop(lexer).text);
    struct ast_loop *tree = ast_create_loop();
    tree->ltype = UNTIL;

    enum parser_status stat = compound_list(&(tree->condition), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_DO)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // eat do
    free(lexer_pop(lexer).text);
    stat = compound_list(&(tree->instruction), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_DONE)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // eat done
    free(lexer_pop(lexer).text);

    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

// 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) {'\n'} 'do'
// compound_list 'done'
static int rule_for(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_for *tree = ast_create_for();
    // eat for
    free(lexer_pop(lexer).text);
    if (lexer_peek(lexer).type < TOKEN_WORD)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // WORD / For "Name" in ...
    tree->name = lexer_pop(lexer).text;
    if (lexer_peek(lexer).type == TOKEN_SEMI)
        free(lexer_pop(lexer).text);

    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        // eat newline
        free(lexer_pop(lexer).text);
    }

    if (lexer_peek(lexer).type == TOKEN_IN)
    {
        // eat in
        free(lexer_pop(lexer).text);
        while (lexer_peek(lexer).type >= TOKEN_WORD)
        {
            size_t s = tree->size;
            if (s == tree->capacity - 1)
            {
                tree->capacity = tree->capacity * 2;
                tree->words =
                    realloc(tree->words, tree->capacity * sizeof(char *));
                memset((tree->words + tree->size), 0,
                       (tree->capacity / 2) * sizeof(char *));
            }
            tree->words[s] = lexer_pop(lexer).text;
            tree->size += 1;
        }

        // eat ; or \n
        free(lexer_pop(lexer).text);
    }

    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        // eat newline
        free(lexer_pop(lexer).text);
    }

    if (lexer_peek(lexer).type != TOKEN_DO)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    // eat do
    free(lexer_pop(lexer).text);

    enum parser_status stat = compound_list(&(tree->instruction), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_DONE)
    {
        // free tree
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    // eat done
    free(lexer_pop(lexer).text);

    // set *ast to tree
    *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

static int rule_case(struct ast_base **ast, struct lexer *lexer)
{
    free(lexer_pop(lexer).text);

    if (lexer_peek(lexer).type < TOKEN_WORD)
        return PARSER_UNEXPECTED_TOKEN;

    struct ast_case *tree = ast_create_case();
    tree->pattern = lexer_pop(lexer).text;

    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        free(lexer_pop(lexer).text);

    if (lexer_peek(lexer).type != TOKEN_IN)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    free(lexer_pop(lexer).text);

    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        free(lexer_pop(lexer).text);

    case_clause(((struct ast_base **)&tree), lexer);
    if (lexer_peek(lexer).type != TOKEN_ESAC)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    free(lexer_pop(lexer).text);

    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

static int case_clause(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_case *tree = (struct ast_case *)*ast;
    enum parser_status stat = case_item(ast, lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;

    tree->items = list_append(tree->items, *ast);

    while (lexer_peek(lexer).type == TOKEN_DSEMI)
    {
        free(lexer_pop(lexer).text);

        while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
            free(lexer_pop(lexer).text);

        stat = case_item(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
            break;

        tree->items = list_append(tree->items, *ast);
    }

    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

static int case_item(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_case_item *tree = ast_create_case_item();
    if (lexer_peek(lexer).type == TOKEN_LPAR)
        free(lexer_pop(lexer).text);

    if (lexer_peek(lexer).type < TOKEN_WORD
        || lexer_peek(lexer).type == TOKEN_ESAC)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    tree->patterns =
        string_list_append(tree->patterns, NULL, lexer_pop(lexer).text);

    while (lexer_peek(lexer).type == TOKEN_PIPE)
    {
        free(lexer_pop(lexer).text);
        if (lexer_peek(lexer).type < TOKEN_WORD)
        {
            ast_free((struct ast_base *)tree);
            tree->patterns =
                string_list_append(tree->patterns, NULL, lexer_pop(lexer).text);
        }
    }

    if (lexer_peek(lexer).type != TOKEN_RPAR)
    {
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    free(lexer_pop(lexer).text);

    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        free(lexer_pop(lexer).text);

    compound_list((struct ast_base **)&tree->instruction, lexer);

    *ast = (struct ast_base *)tree;
    return PARSER_OK;
}

static int prefix(struct ast_base **ast, struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_ASSIGN)
    {
        struct ast_simple_command *tree = (struct ast_simple_command *)*ast;
        tree->assignation =
            string_list_append(tree->assignation, NULL, lexer_pop(lexer).text);
    }

    else
    {
        enum parser_status stat = redirection(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
    }

    return PARSER_OK;
}

// rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
static int rule_if(struct ast_base **ast, struct lexer *lexer)
{
    struct ast_rule_if *tree = ast_create_rule_if();

    // eat the token "if"
    free(lexer_pop(lexer).text);

    enum parser_status stat = compound_list(&(tree->middle), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN || lexer_peek(lexer).type != TOKEN_THEN)
    {
        // free ast
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    // tree->middle = *ast;

    // eat the token "then"
    free(lexer_pop(lexer).text);
    // create rule if tree with the condition of the if

    stat = compound_list(&(tree->left), lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN
        || (lexer_peek(lexer).type != TOKEN_FI
            && lexer_peek(lexer).type != TOKEN_ELSE
            && lexer_peek(lexer).type != TOKEN_ELIF))
    {
        // free ast
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }
    // tree->left = *ast;

    if (lexer_peek(lexer).type == TOKEN_FI)
    {
        // eat fi
        free(lexer_pop(lexer).text);
        *ast = (struct ast_base *)tree;
    }

    // case where the token is else or elif
    else
    {
        stat = else_clause(&(tree->right), lexer);
        if (lexer_peek(lexer).type != TOKEN_FI)
        {
            // free tree
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // eat fi
        free(lexer_pop(lexer).text);
        // tree->right = *ast;
    }

    *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

// else_clause =
//      'else' compound_list
//      | 'elif' compound_list 'then' compound_list [else_clause]
//      ;
static int else_clause(struct ast_base **ast, struct lexer *lexer)
{
    // else case
    if (lexer_peek(lexer).type == TOKEN_ELSE)
    {
        // eat else
        free(lexer_pop(lexer).text);
        enum parser_status stat = compound_list(ast, lexer);

        if (stat == PARSER_UNEXPECTED_TOKEN)
        {
            return PARSER_UNEXPECTED_TOKEN;
        }
    }

    // elif case
    else
    {
        // eat elif
        free(lexer_pop(lexer).text);
        struct ast_rule_if *tree = ast_create_rule_if();

        enum parser_status stat = compound_list(&(tree->middle), lexer);

        if (stat == PARSER_UNEXPECTED_TOKEN
            || lexer_peek(lexer).type != TOKEN_THEN)
        {
            // free ast
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // eat the token "then"
        free(lexer_pop(lexer).text);
        // tree->middle = *ast;

        stat = compound_list(&(tree->left), lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN
            || (lexer_peek(lexer).type != TOKEN_ELSE
                && lexer_peek(lexer).type != TOKEN_ELIF))
        {
            // free tree
            ast_free((struct ast_base *)tree);
            return PARSER_UNEXPECTED_TOKEN;
        }
        // tree->left = *ast;

        // case where it is else or elif
        stat = else_clause(&(tree->right), lexer);
        // tree->right = *ast;
        *ast = (struct ast_base *)tree;
    }

    return PARSER_OK;
}

// compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'};
static int compound_list(struct ast_base **ast, struct lexer *lexer)
{
    while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        free(lexer_pop(lexer).text);
    }

    struct ast_compound_list *tree = ast_create_compound_list();

    enum parser_status stat = and_or(ast, lexer);
    if (stat == PARSER_UNEXPECTED_TOKEN)
    {
        // free ast
        ast_free((struct ast_base *)tree);
        return PARSER_UNEXPECTED_TOKEN;
    }

    tree->commands = list_append(tree->commands, *ast);

    while (lexer_peek(lexer).type == TOKEN_SEMI
           || lexer_peek(lexer).type == TOKEN_NEW_LINE)
    {
        if (lexer_peek(lexer).type == TOKEN_SEMI)
        {
            free(lexer_pop(lexer).text);
        }

        while (lexer_peek(lexer).type == TOKEN_NEW_LINE)
        {
            free(lexer_pop(lexer).text);
        }

        stat = and_or(ast, lexer);
        if (stat == PARSER_UNEXPECTED_TOKEN)
            break;

        tree->commands = list_append(tree->commands, *ast);
    }

    *ast = (struct ast_base *)tree;

    return PARSER_OK;
}

int parser_loop(enum mode mode, char *file, enum parser_status status)
{
    struct lexer *lexer = lexer_new(mode, file);
    struct ast_base *tree = parser_init(&status, lexer);
    int code = ast_eval(tree);
    ast_free(tree);
    while (status != PARSER_EOF)
    {
        tree = parser_init(&status, lexer);
        if (status == PARSER_EOF)
        {
            ast_eval(tree);
            ast_free(tree);
            break;
        }
        if (status == PARSER_NEWLINE)
            continue;
        code = ast_eval(tree);
        ast_free(tree);
    }
    lexer_free(lexer);
    return code;
}
