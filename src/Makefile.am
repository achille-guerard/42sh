SUBDIRS =  \
          parser \
          builtin \
          lexer  \
          io \
          ast \
          expander \
          linked_list \
	  string_list \
	  assigner

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
       parser/libparser.a \
       ast/libast.a \
       builtin/libbuiltin.a \
       lexer/liblexer.a \
       io/libio.a \
       expander/libexpander.a \
       linked_list/liblinked_list.a \
       assigner/libassigner.a \
       string_list/libstring_list.a
