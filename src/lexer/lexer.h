#ifndef LEXER_H
#define LEXER_H

#include "../io/io.h"
#include "token.h"

/**
 * \page Lexer
 *
 * The lexer cuts some input text into block called tokens.

 * This process is done **on demand**: the lexer doesn't read the
 * input more than it needs, and only creates tokens when lexer_peek
 * or lexer_pop is called, and no token is available.
 *
 */

enum quoted
{
    FREE,
    SINGLE_QUOTED,
    DOUBLE_QUOTED,
    BACKSLASHED
};

struct lexer
{
    struct io *reader; // The io struct reading the input
    struct token current_tok;
    struct token next_1;
    struct token next_2;
    enum quoted status; // what is the context
};

void token_pretty_print(enum mode mode, char *file);
struct lexer *lexer_new(enum mode mode, char *file);
void lexer_free(struct lexer *lexer);
struct token parse_input_for_tok(struct lexer *lexer);
struct token lexer_peek(struct lexer *lexer);
struct token lexer_pop(struct lexer *lexer);

#endif /* !LEXER_H */
