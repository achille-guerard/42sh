# Name of the libaries
lib_LIBRARIES = libio.a

# List of file to compile in libio.a
libio_a_SOURCES = io.c \
                      io.h

libio_a_CPPFLAGS = -I$(top_srcdir)/src

libio_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libio.a

