CC = gcc
CFLAGS = -g -O2 -D_REENTRANT -Wall
LIBS = ../libunpipc.a -lrt -lpthread
PROGS = mainpipe

CLEANFILES = core core.* *.core *.o temp.* *.out


all:${PROGS}

mainpipe:mainpipe.o client.o server.o
	$(CC) $(CFLAGS) -o $@ mainpipe.o client.o server.o ${LIBS}

clean:
	rm -f $(PROGS) $(CLEANFILES)

