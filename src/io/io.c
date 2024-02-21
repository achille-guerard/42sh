#define _POSIX_C_SOURCE 200809L
#include "io.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys/stat.h"
#include "unistd.h"

// read all the file and return a string containing the content
struct io *io_init(enum mode mode, char *file)
{
    FILE *fd = NULL;
    if (mode == IO_STRING)
        fd = fmemopen(file, strlen(file), "r");
    else if (mode == IO_FILE)
        fd = fopen(file, "r");
    else // stdin
        fd = stdin;
    if (fd == NULL)
        errx(1, "failed to open file");

    struct io *io = calloc(1, sizeof(struct io));
    io->fd = fd;
    io_getc(io); // init the curr
    return io;
}

// this function is called by the lexer to get the input
signed char io_getc(struct io *io)
{
    io->prev = io->curr;
    io->curr = fgetc(io->fd);
    return io->prev;
}

void io_free(struct io *io)
{
    fclose(io->fd);
    free(io);
}
