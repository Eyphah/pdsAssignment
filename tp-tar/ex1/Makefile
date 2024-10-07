#######################################
### THIS FILE SHOULD NOT BE CHANGED ###
#######################################

CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter
LDFLAGS=-pthread -fsanitize=address

EXE=main_ex1
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

.PRECIOUS: %.o

.PHONY: clean

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE)
	rm -f *~

