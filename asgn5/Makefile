CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS = -g -Wall

OBJS = mytalk.o utility.o server_util.o client_util.o

LIBS = Talk/lib64/libtalk.a -lncurses

all: mytalk
	
mytalk: $(OBJS)
	echo Building mytalk
	$(LD) $(LDFLAGS) -o mytalk $(OBJS) $(LIBS)

mytalk.o: mytalk.c utility.h server_util.h client_util.h
	echo Building mytalk object
	$(CC) $(CFLAGS) -c mytalk.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c utility.c

server_util.o: server_util.c server_util.h utility.h
	echo Building server
	$(CC) $(CFLAGS) -c server_util.c

client_util.o: client_util.c client_util.h utility.h
	echo Building client_util
	$(CC) $(CFLAGS) -c client_util.c

clean: 
	rm *.o

test: all
	echo Testing mytalk
