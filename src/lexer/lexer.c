#define _POSIX_C_SOURCE 200809L
#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../io/io.h"
#include "token.h"

// TODO : move the helpers in another file

static char *look_over_there[] = {
    "\n",
    "&&",
    "(",
    ")",
    "|",
    "||",
    ";",
    ";;",
    "<",
    ">",
    "<<",
    ">>",
    ">|",
    "<&",
    ">&",
    "<>",
    "{",
    "}",
    "!",
    "EOF",
    "ERROR",
    "WORD but you should not retreve value here",
    "ASSIGN but you should not retreive it here",
    "IO_NUMBER but you should not retreve value here",
    "do",
    "done",
    "if",
    "fi",
    "then",
    "elif",
    "else",
    "case",
    "esac",
    "while",
    "until",
    "for",
    "in",
    NULL
};

// HELPERS BELOW HERE :

static int is_ope_redir(char a)
{
    if (a == '<' || a == '>')
        return 1;
    else
        return 0;
}

static int char_is_operator(char a)
{
    if (a == '\n' || a == '&' || a == '(' || a == ')' || a == '|' || a == ';'
        || a == '<' || a == '>')
        return 1;
    else
        return 0;
}

static int char_is_quoting(char a)
{
    if (a == '\'' || a == '\"' || a == '\\')
        return 1;
    else
        return 0;
}

// RULE 5
static int valid_naming(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else if (c == '_')
        return 1;
    else
        return 0;
}

static int numeric(char *text)
{
    size_t i = 0;
    while (text[i] != '\0')
    {
        if (text[i] < '0' || text[i] > '9')
            return 0;
        i++;
    }
    return 1;
}

static int is_assignment(char *text)
{
    int i = 0;
    while (text[i] != '=' && text[i] != EOF)
    {
        if (!valid_naming(text[i++]))
            return 0;
    }
    if (text[i] == '=')
        return 1;
    else
        return 0;
}

static signed char current(struct lexer *lexer)
{
    return lexer->reader->curr;
}

// NUNCA LLAMAR ESTA FUNCION SIN VERIFICAR EL INDEX
static char previous(struct lexer *lexer)
{
    return lexer->reader->prev;
}

static struct token create_eof(char *text)
{
    free(text);
    struct token tok;
    tok.type = TOKEN_EOF;
    return tok;
}

// RULE 3
static int word_is_operator(char *text, size_t len)
{
    size_t i = 0;
    if (len == 1)
        return char_is_operator(text[0]);
    while (i < 15 && look_over_there[i] != NULL) // i < 14 for operator only
    {
        if (strcmp(text, look_over_there[i++]) == 0)
            return 1;
    }
    return 0;
}

//  states if a and be are part of the same operator or not
static int complete(char a, char b)
{
    if (a == '&' && b == '&')
        return 1;
    else if (a == '|' && b == '|')
        return 1;
    else if (a == ';' && b == ';')
        return 1;
    else if (a == '<')
    {
        if (b == '<' || b == '&' || b == '>')
            return 1;
        else
            return 0;
    }
    else if (a == '>')
    {
        if (b == '>' || b == '&' || b == '|')
            return 1;
        else
            return 0;
    }
    else
        return 0; // for now always false
}

// RULE 4
static void handle_quoting(struct lexer *lexer, char *text, int i)
{
    if (current(lexer) == '\\')
        lexer->status = BACKSLASHED;

    else if (current(lexer) == '\'')
    {
        if (lexer->status == SINGLE_QUOTED)
            lexer->status = FREE;
        else
            lexer->status = SINGLE_QUOTED;
    }
    else
    {
        if (lexer->status == DOUBLE_QUOTED)
            lexer->status = FREE;
        else
            lexer->status = DOUBLE_QUOTED;
    }

    text[i] = io_getc(lexer->reader);
}

// RULE 4
static int test_rule4(struct lexer *lexer)
{
    if (lexer->status == FREE)
        return 1;

    // laissez sortir les quotes qui matchent
    else if (lexer->status == SINGLE_QUOTED && current(lexer) == '\'')
        return 1;

    else if (previous(lexer) != '\\' && lexer->status == DOUBLE_QUOTED
             && current(lexer) == '\"')
        return 1;

    else
        return 0;
}

static int is_dolla(char c)
{
    if (c == '$' || c == '`')
        return 1;
    else
        return 0;
}

static int is_closing(char c)
{
    if (c == ')' || c == '`')
        return 1;
    else
        return 0;
}

static int is_opening(char c)
{
    if (c == '(' || c == '`')
        return 1;
    else
        return 0;
}

static void recursive_check(struct lexer *lexer, char *text, size_t *i)
{
    int rec_nb = 1;
    while (rec_nb != 0 && current(lexer) != EOF)
    {
        if (is_closing(current(lexer)))
            rec_nb--;

        else if (is_opening(current(lexer)))
            rec_nb++;

        if (rec_nb != 0)
            text[(*i)++] = io_getc(lexer->reader);
    }
}

// RULE 5
static void handle_dolla(struct lexer *lexer, char *text, size_t *i)
{
    // first we eat the dolla
    int backquoted = (current(lexer) == '`') ? 1 : 0;
    text[(*i)++] = io_getc(lexer->reader);
    int braqueted = (current(lexer) == '{') ? 1 : 0;
    int parenthesised = (current(lexer) == '(') ? 1 : 0;

    if (braqueted || parenthesised) // we eat the bracket
        text[(*i)++] = io_getc(lexer->reader);

    if (parenthesised || backquoted) // command substitution
    {
        recursive_check(lexer, text, i);

        if (parenthesised)
        {
            if (current(lexer) == ')') // we eaaat
                text[(*i)++] = io_getc(lexer->reader);
            else
                errx(1, "lexer did not match ending parenthesis");
        }
        else
        {
            if (current(lexer) == '`') // we eaaat
                text[(*i)++] = io_getc(lexer->reader);
            else
                errx(1, "lexer did not match ending parenthesis");
        }
    }
    else // classic variables
    {
        while (valid_naming(current(lexer)))
            text[(*i)++] = io_getc(lexer->reader);

        if (braqueted)
        {
            if (current(lexer) == '}') // we eaaat
                text[(*i)++] = io_getc(lexer->reader);
            else
                errx(1, "lexer did not match ending braquet");
        }
    }
}

// RULE 8 AND 10
static void add_char_checks(struct lexer *lexer, char *text, size_t *i)
{
    // 1st check we dont add a backslashed newline
    if (current(lexer) == '\n' && previous(lexer) == '\\')
    {
        text[--(*i)] = '\0';
        io_getc(lexer->reader);
    }
    // 2nd check we dont add a quoted newline
    // else if (lexer->status != FREE && current(lexer) == '\n')
    //     io_getc(lexer->reader);

    else
        text[(*i)++] = io_getc(lexer->reader);
}

// RULE 9
static void skip_comment(struct lexer *lexer)
{
    while (current(lexer) != '\n' && current(lexer) != EOF)
    {
        io_getc(lexer->reader);
    }
}

// DELIMITERS FUNCTIONS

static enum token_type pas_chetor(char *text, struct lexer *lexer)
{
    size_t i = 0;
    while (look_over_there[i] != NULL)
    {
        if (strcmp(text, look_over_there[i]) == 0)
            return i;
        i++;
    }
    signed char t0 = text[0];
    if (t0 == EOF)
        return TOKEN_EOF;
    else if (numeric(text) && is_ope_redir(current(lexer)))
        return TOKEN_IO_NUM;
    else if (is_assignment(text))
        return TOKEN_ASSIGN;
    else
        return TOKEN_WORD;
}

static struct token delimit(struct lexer *lexer, char *text)
{
    if (lexer->status == BACKSLASHED)
        lexer->status = FREE;
    if (current(lexer) == EOF && lexer->status != FREE)
    {
        free(text);
        fprintf(stderr, "unexpected EOF while looking for matching quote\n");
        exit(2);
    }

    struct token res;
    res.type = pas_chetor(text, lexer);
    res.text = strdup(text);

    free(text);

    if (isblank(current(lexer)))
        io_getc(lexer->reader);

    return res;
}

/**
 * \brief Returns a token from the input string
 * This function goes through the input string character by character and
 * builds a token. lexer_peek and lexer_pop should call it. If the input is
 * invalid, you must print something on stderr and return the appropriate token.
 */
struct token parse_input_for_tok(struct lexer *lexer)
{
    // this text is then realloced if a WORD or freed if not a word
    //  in the delimit function
    char *text = calloc(1001, sizeof(char));
    size_t i = 0;
    while (1)
    {
        if (i != 0 && i % 1000 == 0)
            text = realloc(text, i + 1001);

        if (current(lexer) == EOF) // rule 1
        {
            if (i == 0)
                return create_eof(text);
            else
                return delimit(lexer, text);
        }
        else if (lexer->status == FREE && i != 0
                 && word_is_operator(text, i)) // rule 2 and 3
        {
            if (complete(previous(lexer), current(lexer))) // rule 2
                text[i++] = io_getc(lexer->reader);

            return delimit(lexer, text);
        }
        else if (test_rule4(lexer) && char_is_quoting(current(lexer))) // rule4
        {
            handle_quoting(lexer, text, i++);
        }
        else if (lexer->status == FREE && is_dolla(current(lexer)))
        {
            handle_dolla(lexer, text, &i);
        }
        else if (lexer->status == FREE
                 && char_is_operator(current(lexer))) // rule 6
        {
            if (i != 0) // if there is something to delimit
                return delimit(lexer, text);
            else
                text[i++] = io_getc(lexer->reader);
        }
        else if (lexer->status == FREE && isblank(current(lexer))) // rule 7
        {
            if (i != 0)
            {
                return delimit(lexer, text);
            }
            else
                io_getc(lexer->reader);
        }
        else if (i != 0) // rule 8
        {
            // if we encounter a newline while quoted, FUCK IT
            add_char_checks(lexer, text, &i);
        }
        else if (lexer->status == FREE && current(lexer) == '#') // rule 9
        {
            skip_comment(lexer);
        }
        else // rule 10
        {
            add_char_checks(lexer, text, &i);
        }
        if (lexer->status == BACKSLASHED && previous(lexer) != '\\')
            lexer->status = FREE;
    }
}

// EXPORTED FUNCTIONS :

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result. This functions is
 * meant to help the parser check if the next token matches some rule.
 */
struct token lexer_peek(struct lexer *lexer)
{
    return lexer->current_tok;
}

/**
 * \brief Returns the current token, then parse the next one and store it
          inside lexer->current_tok
 */
struct token lexer_pop(struct lexer *lexer)
{
    struct token res = lexer->current_tok;
    lexer->current_tok = lexer->next_1;
    lexer->next_1 = lexer->next_2;
    if (lexer->next_2.type != TOKEN_EOF)
        lexer->next_2 = parse_input_for_tok(lexer);
    return res;
}

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_new(enum mode mode, char *file)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    res->reader = io_init(mode, file);
    res->status = FREE; // we start as unquoted
    res->current_tok = parse_input_for_tok(res);
    res->next_1 = parse_input_for_tok(res);
    res->next_2 = parse_input_for_tok(res);
    return res;
}

/**
 ** \brief Free the given lexer, and its input.
 */
void lexer_free(struct lexer *lexer)
{
    io_free(lexer->reader);
    free(lexer);
}

/**
 * \brief Prints all the tokens of the string input.
 */
void token_pretty_print(enum mode mode, char *file)
{
    struct lexer *lexer = lexer_new(mode, file);
    struct token token = lexer_pop(lexer);

    while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR)
    {
        if (token.type == TOKEN_WORD)
            printf("WORD: %s\n", token.text);
        else
            printf("%s\n", look_over_there[token.type]);

        token = lexer_pop(lexer);
    }

    if (token.type == TOKEN_EOF)
        printf("EOF\n");

    lexer_free(lexer);
}
