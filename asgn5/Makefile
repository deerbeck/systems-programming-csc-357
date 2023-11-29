CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS = -g -Wall

OBJS = mytalk.o utility.o

LIBS = Talk/lib64/libtalk.a -lncurses

all: mytalk
	
mytalk: $(OBJS)
	echo Building mytalk
	$(LD) $(LDFLAGS) -o mytalk $(OBJS) $(LIBS)

mytalk.o: mytalk.c utility.h 
	echo Building mytalk object
	$(CC) $(CFLAGS) -c mytalk.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c utility.c

clean: 
	rm *.o

test: all
	echo Testing mytalk
