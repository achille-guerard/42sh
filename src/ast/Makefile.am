# Name of the libaries
lib_LIBRARIES = libast.a

# List of file to compile in libast.a
libast_a_SOURCES = ast.h \
                   ast_create_1.c \
		   ast_create_2.c \
		   ast_create_3.c \
                   ast_free_1.c \
		   ast_free_2.c \
		   ast_free_3.c \
		   ast_evaluate_1.c \
		   ast_evaluate_2.c \
		   ast_evaluate_3.c


libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libast.a

