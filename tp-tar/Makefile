CC      = gcc
CFLAGS  = -Wall -Wextra -fsanitize=thread
CFLAGS += -g
LDFLAGS = -pthread -fsanitize=thread

.PHONY: all lib test

all:
	make -C ex1
	make -C ex2

lib:
	make -C lib

test: all lib test
	make -C test test 

clean:
	make -C ex1 clean
	make -C ex2 clean
	make -C test clean
	rm -rf *~
