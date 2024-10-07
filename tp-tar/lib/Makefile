CC      = gcc
CFLAGS  = -Wall -Wextra -fsanitize=thread
CFLAGS += -g
LDFLAGS = -pthread -fsanitize=thread

libpdstest.a : pdstestlib.o tlogger.o
	ar -crs libpdstest.a pdstestlib.o tlogger.o

pdstestlib.o : pdstestlib.c pdstestlib.h
	$(CC) -c $(CFLAGS) $<

tlogger.o : tlogger.c tlogger.h pdstestlib.h
	$(CC) -c $(CFLAGS) $<


clean :
	rm -f *.a
	rm -f *.o
	rm -f *~
