CC=clang
CFLAGS= -Wall -g -pedantic -std=c11 -Werror -D_DEFAULT_SOURCE
VPATH=../../

.PHONY: all clean

all: display_test

display_test: display_test.o display.o file_utils.o formatter.o linked_list.o string_utils.o

clean:
	rm -f *.o
	rm -f display_test
