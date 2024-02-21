# Name of the libaries
lib_LIBRARIES = libassigner.a

# List of file to compile in liblinked_list.a
libassigner_a_SOURCES = assigner_var.c \
			assigner_func.c \
                      	assigner.h

libassigner_a_CPPFLAGS = -I$(top_srcdir)/src

libassigner_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libassigner.a

