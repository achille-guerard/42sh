# Name of the libaries
lib_LIBRARIES = liblexer.a

# List of file to compile in liblexer.a
liblexer_a_SOURCES = lexer.c \
                      lexer.h

lib_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexer.a

