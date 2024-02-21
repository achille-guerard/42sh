#ifndef IO_H
#define IO_H
#include <stdio.h>

enum mode
{
    IO_FILE,
    IO_STRING,
    IO_STDIN
};

struct io
{
    FILE *fd; // file descriptor
    signed char curr; // current char
    signed char prev; // previous char
};

struct io *io_init(enum mode mode, char *file);
signed char io_getc(struct io *io);
void io_free(struct io *io);

#endif /* !IO_H */
