CC = gcc

CFLAGS = -g -Wall

LD = gcc

LDFLAGS = -g -Wall

OBJS = mytalk.o utility.o

LIBS = -ltalk -lncurses

all: mytalk
	
mytalk: $(OBJS)
	echo Building mytalk
	$(LD) $(LDFLAGS) -o mytalk -L ~/Documents/systems-programming-csc-357/asgn5/Talk/lib64 $(OBJS) $(LIBS)

mytalk.o: mytalk.c utility.h 
	echo Building mytalk object
	$(CC) $(CFLAGS) -c -I ~/Documents/systems-programming-csc-357/asgn5/Talk/include mytalk.c

utility.o: utility.c utility.h
	echo Building utility
	$(CC) $(CFLAGS) -c -I ~/Documents/systems-programming-csc-357/asgn5/Talk/include utility.c

clean: 
	rm *.o

test: all
	echo Testing mytalk
