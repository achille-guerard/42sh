SUBDIRS = src\
	  tests

leaks: CFLAGS += -O0 -g -fsanitize=address
leaks: LDFLAGS += -fsanitize=address
leaks: all

coverage: check
	$(RM) a-conftest.gcno
	gcovr --html --output=report.html --html-details
	firefox report.html
