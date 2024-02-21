#ifndef EXPANDER_H
#define EXPANDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"

int valid_naming(char c);
char *string_expander(char *text, int len);

#endif /* !EXPANDER_H */
