CC      = gcc
CFLAGS  = -g -Wall -pedantic -std=c11
VPATH   = ../../

.SUFFIXES: .c .o

naive_bayes: naive_bayes.o naive_bayes.h map.o map.h large_double.o large_double.h
	$(CC) $(CFLAGS) -o naive_bayes naive_bayes.o map.o linked_list.o large_double.o -lm
naive_bayes.o: naive_bayes.h map.o map.h linked_list.h linked_list.o large_double.o large_double.h
map.o: map.c map.h linked_list.o linked_list.h
linked_list.o: linked_list.h
large_double.o: large_double.h

clean:
	rm -f naive_bayes
	rm -f *.o
.PHONY: clean
