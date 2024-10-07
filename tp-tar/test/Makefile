CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -I ../lib
LDFLAGS=-fsanitize=thread -L ../lib
LIBS=-pthread -lpdstest

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_EXES=test_ex1 test_ex2 

.PRECIOUS: %.o

.PHONY: all clean test

all : $(TEST_EXES) $(PRGS)

test_ex1.o: test_ex1.c ../ex1/answers.h
	$(CC) $(CFLAGS) -I ../ex1/ -c -o $@ $<

test_ex2.o: test_ex2.c ../ex2/answers.h
	$(CC) $(CFLAGS) -I ../ex2/ -c -o $@ $<

test_ex1: test_ex1.o ../ex1/answers.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) 

test_ex2: test_ex2.o ../ex2/answers.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) 

test : all
	./run_tests.sh

runtests:
	./test_ex1
	./test_ex2

clean :
	rm -f *.o
	rm -f *~
	rm -f $(TEST_EXES)

