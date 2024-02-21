# Name of the libaries
lib_LIBRARIES = libbuiltin.a

# List of file to compile in libbuiltin.a
libbuiltin_a_SOURCES = builtin.h \
		   builtin.c \
		   builtin_cd.c \
		   builtin_dot.c


libbuiltin_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltin_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltin.a

