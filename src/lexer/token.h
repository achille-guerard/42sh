#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    // CONTROL OPERATORS
    TOKEN_NEW_LINE, // '\n'
    TOKEN_AND_IF, // &&
    TOKEN_LPAR, // (
    TOKEN_RPAR, // )
    TOKEN_PIPE, // |
    TOKEN_OR_IF, // ||
    TOKEN_SEMI, // ;
    TOKEN_DSEMI, // ;;

    // REDIRECTION OPERATORS
    TOKEN_LESS, // <
    TOKEN_GREAT, // >
    TOKEN_DLESS, // <<
    TOKEN_DGREAT, // >>
    TOKEN_GREATPIPE, // >|
    TOKEN_LESSAND, // <&
    TOKEN_GREATAND, // >&
    TOKEN_LESSGREAT, // <>

    // WEIRD STATUS
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // }
    TOKEN_BANG, // !

    TOKEN_EOF, // end of file
    TOKEN_ERROR, // it is not a real token, it is returned in case of invalid
                 // input

    // MAIN ONE
    TOKEN_WORD, // e.g. 'echo'
    TOKEN_ASSIGN, // e.g. 'a=3'

    // NUMBER
    TOKEN_IO_NUM, // 1 2 3 ...

    // RESERVED WORD
    TOKEN_DO, // 'do'
    TOKEN_DONE, // 'done'
    TOKEN_IF, // 'if'
    TOKEN_FI, // 'fi'
    TOKEN_THEN, // 'then'
    TOKEN_ELIF, // 'elif'
    TOKEN_ELSE, // 'else'

    TOKEN_CASE, // 'case'
    TOKEN_ESAC, // 'esac'
    TOKEN_WHILE, // 'while'
    TOKEN_UNTIL, // 'until'
    TOKEN_FOR, // 'for'

    TOKEN_IN // 'in'
};

struct token
{
    enum token_type type; // The kind of token
    char *text; // only if the token is a word
};

#endif /* !TOKEN_H */
