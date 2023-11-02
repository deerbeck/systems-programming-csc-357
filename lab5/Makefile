CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS =

OBJS = mypwd.o

all: mypwd
	
mypwd: $(OBJS)
	echo Building mypwd
	$(LD) $(LDFLAGS) -o mypwd $(OBJS)


mypwd.o: mypwd.c 
	echo Building mypwd
	$(CC) $(CFLAGS) -c mypwd.c

clean: 
	rm *.o
